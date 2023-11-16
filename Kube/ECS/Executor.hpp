/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: System executor
 */

#pragma once

#include <Kube/Core/Expected.hpp>
#include <Kube/Core/MPSCQueue.hpp>
#include <Kube/Core/TrivialFunctor.hpp>
#include <Kube/Core/UniquePtr.hpp>
#include <Kube/Flow/Scheduler.hpp>

#include "System.hpp"
#include "PipelineEvent.hpp"

#ifndef KUBE_ECS_PIPELINE_CACHE_COUNT
# define KUBE_ECS_PIPELINE_CACHE_COUNT 4
#endif

#ifndef KUBE_ECS_PER_PIPELINE_SYSTEM_CACHE_COUNT
# define KUBE_ECS_PER_PIPELINE_SYSTEM_CACHE_COUNT 4
#endif

namespace kF::ECS
{
    class Executor;

    /** @brief Unique pointer to system */
    using SystemPtr = Core::UniquePtr<Internal::ASystem, ECSAllocator>;

    namespace Internal
    {
        /** @brief Explicit dependency before target system */
        template<typename SystemType>
        struct TagBefore
        {
            using Type = SystemType;
            static constexpr bool After = false;
        };

        /** @brief Explicit dependency after target system */
        template<typename SystemType>
        struct TagAfter
        {
            using Type = SystemType;
            static constexpr bool After = true;
        };
    }

    /** @brief Helper used to set explicit dependency before a system */
    template<typename SystemType>
    constexpr auto RunBefore = Internal::TagBefore<SystemType>();

    /** @brief Helper used to set explicit dependency after a system */
    template<typename SystemType>
    constexpr auto RunAfter = Internal::TagAfter<SystemType>();
}

class alignas_double_cacheline kF::ECS::Executor
{
public:
    /** @brief Optimized cache number of pipelines inside executor */
    constexpr static std::size_t OptimalPipelinePerPipelineCount = KUBE_ECS_PIPELINE_CACHE_COUNT;

    /** @brief Optimized cache number of systems in per pipeline */
    constexpr static std::size_t OptimalSystemPerPipelineCount = KUBE_ECS_PER_PIPELINE_SYSTEM_CACHE_COUNT;

    /** @brief Default number of events in pipeline event queue */
    constexpr static std::size_t DefaultPipelineEventQueueSize = 4096 / sizeof(PipelineEvent);

    /** @brief Pipeline begin pass (if returns false, the whole pipeline is ignored) */
    using PipelineBeginPass = Core::TrivialFunctor<bool(void)>;

    /** @brief Small vector optimized for pipeline storage */
    template<typename Type>
    using PipelineSmallVector = Core::SmallVector<Type, OptimalPipelinePerPipelineCount, ECSAllocator>;

    /** @brief Small vector optimized for system storage per pipeline */
    template<typename Type>
    using SystemSmallVector = Core::SmallVector<Type, OptimalSystemPerPipelineCount, ECSAllocator>;

    /** @brief Store the system names of a pipeline */
    using PipelineSystemNames = SystemSmallVector<Core::HashedName>;

    /** @brief Store the systems of a pipeline */
    using PipelineSystems = SystemSmallVector<SystemPtr>;

    /** @brief Store the events of a pipeline */
    using PipelineEvents = Core::UniquePtr<Core::MPSCQueue<PipelineEvent, ECSAllocator>, ECSAllocator>;

    /** @brief Structure of array of pipelines data */
    struct alignas_double_cacheline Pipeline
    {
        // Properties
        Core::HashedName hash {};
        bool isTimeBound {};
        std::int64_t tickRate {};
        PipelineBeginPass beginPass {};
        PipelineEvents events {};
        PipelineSystemNames systemHashes {};
        PipelineSystems systems {};
        std::string_view name {};
        std::thread thread {};
        Flow::Graph graph {};
    };
    static_assert_alignof_double_cacheline(Pipeline);

    /** @brief Unique pointer to pipeline */
    using PipelinePtr = Core::UniquePtr<Pipeline, ECSAllocator>;


    /** @brief Get global instance */
    [[nodiscard]] static inline Executor &Get(void) noexcept { return *_Instance; }


    /** @brief Destructor */
    ~Executor(void) noexcept;

    /** @brief Construct scheduler with a maximum amount of workers, tasks and events */
    Executor(
        const std::size_t workerCount = Flow::Scheduler::AutoWorkerCount,
        const std::size_t taskQueueSize = DefaultPipelineEventQueueSize
    ) noexcept;


    /** @brief Get reference to graph scheduler */
    [[nodiscard]] inline Flow::Scheduler &scheduler(void) noexcept { return _scheduler; }
    [[nodiscard]] inline const Flow::Scheduler &scheduler(void) const noexcept { return _scheduler; }


    /** @brief Run the executor in blocking mode */
    void run(
        const Core::HashedName mainThreadPipelineHash = {},
        Core::TrivialFunctor<void(void)> &&mainThreadInlineTick = {}
    ) noexcept;

    /** @brief Stop the executor */
    inline void stop(void) noexcept { _running.store(false, std::memory_order_relaxed); }


    /** @brief Add a pipeline into executor
     *  @param If event queue size is zero, then it is remplaced by DefaultPipelineEventQueueSize */
    template<kF::ECS::Pipeline PipelineType, PipelineTimeMode TimeMode = PipelineTimeMode::Free, typename BeginPass = PipelineBeginPass>
    void addPipeline(
        const std::int64_t frequencyHz,
        BeginPass &&beginPass = PipelineBeginPass {},
        const std::size_t eventQueueSize = {}
    ) noexcept;

    /** @brief Add a system into executor (system's pipeline must be setup before) */
    template<typename SystemType, auto ...Dependencies, typename ...Args>
        requires std::derived_from<SystemType, kF::ECS::Internal::ASystem>
    SystemType &addSystem(Args &&...args) noexcept;


    /** @brief Get pipeline index from pipeline runtime name */
    [[nodiscard]] Core::Expected<PipelineIndex> getPipelineIndex(const Core::HashedName pipelineHash) const noexcept;

    /** @brief Get system index from pipeline index and runtime name */
    [[nodiscard]] Core::Expected<PipelineIndex> getSystemIndex(const PipelineIndex pipelineIndex, const Core::HashedName systemHash) const noexcept;


    /** @brief Get pipeline tick rate from pipeline index */
    [[nodiscard]] inline std::int64_t getPipelineTickRate(const PipelineIndex pipelineIndex) const noexcept
        { return _pipelines.at(pipelineIndex)->tickRate; }

    /** @brief Get pipeline tick rate from pipeline index */
    inline void setPipelineTickRate(const PipelineIndex pipelineIndex, const std::int64_t frequencyHz) noexcept
        { _pipelines.at(pipelineIndex)->tickRate = std::int64_t(HzToRate(frequencyHz)); }

    /** @brief Get pipeline time bound state from pipeline index */
    [[nodiscard]] inline bool isPipelineTimeBound(const PipelineIndex pipelineIndex) const noexcept
        { return _pipelines.at(pipelineIndex)->isTimeBound; }


    /** @brief Get system reference
     *  @note Abort if system doesn't exist */
    template<typename SystemType>
    [[nodiscard]] SystemType &getSystem(void) noexcept;

    /** @brief Get system reference using pipeline index
     *  @note Abort if system doesn't exist */
    template<typename SystemType>
    [[nodiscard]] SystemType &getSystem(const PipelineIndex pipelineIndex) noexcept;

    /** @brief Get opaque system using pipeline & system indexes
     *  @note Doesn't check anything */
    [[nodiscard]] inline Internal::ASystem *getSystemOpaque(const PipelineIndex pipelineIndex, const PipelineIndex systemIndex) noexcept
        { return _pipelines.at(pipelineIndex)->systems.at(systemIndex).get(); }


    /** @brief Send an event to a system */
    template<typename DestinationPipeline, bool RetryOnFailure = true, typename Callback>
    void sendEvent(Callback &&callback) noexcept;

    /** @brief Send an event to a system, using a specific pipeline index
     *  @note DestinationPipeline and pipelineIndex must match */
    template<bool RetryOnFailure = true, typename Callback>
    void sendEvent(const PipelineIndex pipelineIndex, Callback &&callback) noexcept;


private:
    /** @brief Tick the executor once
     *  @param inlineTick Is a functor called on the pipeline scheduling thread */
    void runPipeline(Pipeline &pipeline, Core::TrivialFunctor<void(void)> &&inlineTick = {}) noexcept;

    /** @brief Build a pipeline graph given its index */
    void buildPipelineGraph(Pipeline &pipeline) noexcept;


    // Global access instance
    static Executor *_Instance;

    PipelineSmallVector<Core::HashedName> _pipelineHashes {};
    PipelineSmallVector<PipelinePtr> _pipelines {};
    alignas_double_cacheline std::atomic_bool _running {};
    Flow::Scheduler _scheduler {};
};
static_assert_alignof_double_cacheline(kF::ECS::Executor);

#include "Executor.ipp"