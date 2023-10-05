/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: System executor
 */

#include <thread>

#include <Kube/Core/FunctionDecomposer.hpp>

#include "Executor.hpp"

template<kF::ECS::Pipeline PipelineType, kF::ECS::PipelineTimeMode TimeMode, typename BeginPass, typename InlineBeginPass>
inline void kF::ECS::Executor::addPipeline(const std::int64_t frequencyHz, const std::size_t eventQueueSize,
        BeginPass &&beginPass, InlineBeginPass &&inlineBeginPass) noexcept
{
    kFEnsure(frequencyHz >= 0, "ECS::Executor::addPipeline: Pipeline only support frequency in range [0, inf[");

    _pipelines.hashes.push(PipelineType::Hash);
    _pipelines.systemHashes.push();
    _pipelines.systems.push();
    _pipelines.events.push(eventQueueSize ? PipelineEvents::Make(eventQueueSize) : PipelineEvents());
    _pipelines.clocks.push(PipelineClock {
        .maskedTickRate = [tickRate = HzToRate(frequencyHz)] {
            if constexpr (TimeMode == PipelineTimeMode::Bound)
                return tickRate | PipelineClock::TimeBoundMask;
            else
                return tickRate;
        }(),
    });
    _pipelines.graphs.push(PipelineGraph::Make());
    _pipelines.inlineBeginPasses.push(std::forward<InlineBeginPass>(inlineBeginPass));
    _pipelines.beginPasses.push(std::forward<BeginPass>(beginPass));
    _pipelines.names.push(PipelineType::Name);
}

template<typename SystemType, auto ...Dependencies, typename ...Args>
    requires std::derived_from<SystemType, kF::ECS::Internal::ASystem>
inline SystemType &kF::ECS::Executor::addSystem(Args &&...args) noexcept
{
    using Pipeline = typename SystemType::ExecutorPipeline;

    // Get system's pipeline index
    const auto expected = getPipelineIndex(Pipeline::Hash);
    kFEnsure(expected.success(),
        "ECS::Executor::addSystem: System '", SystemType::Name, "' requires pipeline '",
        SystemType::ExecutorPipeline::Name, "' not found");

    // Determine system insert position
    auto &systems = _pipelines.systems.at(*expected);
    auto &systemNames = _pipelines.systemHashes.at(*expected);
    auto insertAt = systemNames.end();
    // Scan dependencies
    if constexpr (sizeof...(Dependencies) != 0) {
        kFEnsure(insertAt != nullptr,
            "ECS::Executor::addSystem: System '", SystemType::Name, "' is added before its dependencies");
        auto orderFunc = [&systemNames]<typename DependencyHolder>(auto &insertAt, DependencyHolder) {
            // Deduce dependency type
            using Dependency = DependencyHolder::Type;

            // Find dependency at runtime
            const auto dependencyIt = systemNames.find(Dependency::Hash);
            kFEnsure(dependencyIt != systemNames.end(),
                "ECS::Executor::addSystem: Dependency '", Dependency::Name, "' of system '", SystemType::Name, "' not found");
            // Ensure iterator met dependency requirement
            if constexpr (DependencyHolder::After) {
                if (insertAt <= dependencyIt) {
                    insertAt = dependencyIt + 1;
                    return true;
                }
            } else {
                if (insertAt > dependencyIt) {
                    insertAt = dependencyIt;
                    return true;
                }
            }
            return false;
        };
        // For each dependency, run order functor
        if ((orderFunc(insertAt, Dependencies) || ...)) {
            // Run twice and ensure the iterator is stable
            kFEnsure(!(orderFunc(insertAt, Dependencies) || ...),
                "ECS::Executor::addSystem: System '", SystemType::Name, "' have circular dependencies");
        }
    }

    // Insert system & system name
    const auto insertIndex = std::distance(systemNames.begin(), insertAt);
    systemNames.insert(
        insertAt,
        SystemType::Hash
    );
    const auto systemIt = systems.insert(
        systems.begin() + insertIndex,
        SystemPtr::Make<SystemType>(std::forward<Args>(args)...)
    );

    // Return reinterpreted system
    return reinterpret_cast<SystemType &>(**systemIt);
}

template<typename SystemType>
inline SystemType &kF::ECS::Executor::getSystem(void) noexcept
{
    const auto pipelineIndex = getPipelineIndex(SystemType::ExecutorPipeline::Hash);
    kFEnsure(pipelineIndex.success(),
        "ECS::Executor::getSystem: Couldn't find pipeline '", SystemType::ExecutorPipeline::Name, "' required by system '", SystemType::Name, '\'');
    return getSystem<SystemType>(pipelineIndex.value());
}

template<typename SystemType>
inline SystemType &kF::ECS::Executor::getSystem(const std::uint32_t pipelineIndex) noexcept
{
    const auto systemIndex = getSystemIndex(pipelineIndex, SystemType::Hash);
    kFEnsure(systemIndex.success(),
        "ECS::Executor::getSystem: Couldn't find system '", SystemType::Name, "' inside pipeline '", SystemType::ExecutorPipeline::Name, '\'');
    return *reinterpret_cast<SystemType *>(_pipelines.systems.at(pipelineIndex).at(systemIndex.value()).get());
}

template<typename DestinationPipeline, bool RetryOnFailure, typename Callback>
inline void kF::ECS::Executor::sendEvent(Callback &&callback) noexcept
{
    using Decomposer = Core::FunctionDecomposerHelper<Callback>;

    static_assert(Decomposer::IndexSequence.size() <= 1,
        "ECS::Executor::sendEvent: Event callback may only have one argument that must be a reference to any system");

    // Query destination pipeline index
    const auto pipelineIndex = getPipelineIndex(DestinationPipeline::Hash);
    kFEnsure(pipelineIndex.success(),
        "ECS::Executor::getSystem: Couldn't find pipeline '", DestinationPipeline::Name, '\'');
    sendEvent<RetryOnFailure>(*pipelineIndex, std::forward<Callback>(callback));
}

template<bool RetryOnFailure, typename Callback>
inline void kF::ECS::Executor::sendEvent(const std::uint32_t pipelineIndex, Callback &&callback) noexcept
{
    using Decomposer = Core::FunctionDecomposerHelper<Callback>;

    PipelineEvent pipelineEvent;

    // If the callback has a system reference as single argument
    if constexpr (Decomposer::IndexSequence.size() == 1) {
        // Retreive callback argument
        using DestinationSystem = std::tuple_element_t<0, typename Decomposer::ArgsTuple>;
        using FlatSystem = std::remove_reference_t<DestinationSystem>;
        static_assert(std::is_reference_v<DestinationSystem>,
            "ECS::Executor::sendEvent: Event callback invalid argument, must be a reference to any system");

        // Query destination system index and wrap callback
        pipelineEvent = [callback = std::forward<Callback>(callback), system = getSystem<FlatSystem>()] {
            callback(system);
        };
    } else
        pipelineEvent = std::forward<Callback>(callback);

    // When RetryOnFailure is set to true, loop until event is pushed
    bool res = false;
    while (true) {
        res = _pipelines.events.at(pipelineIndex)->push<true>(pipelineEvent);
        if constexpr (RetryOnFailure) {
            if (!res) {
                std::this_thread::yield();
                continue;
            }
        }
        break;
    }
    kFEnsure(res, "Executor::sendEvent: Critical error, event queue is full");
}