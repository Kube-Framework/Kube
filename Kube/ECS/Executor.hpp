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
    /** @brief Executor event */
    using ExecutorEvent = Core::TrivialFunctor<bool(void)>;

    /** @brief Pipeline begin pass (if returns false, the whole pipeline is ignored) */
    using PipelineBeginPass = Core::TrivialFunctor<bool(void)>;


    /** @brief Default number of events in pipeline event queue */
    constexpr static std::size_t DefaultPipelineEventQueueSize = 4096 / sizeof(PipelineEvent);

    /** @brief Default number of events in executor event queue */
    constexpr static std::size_t DefaultExecutorEventQueueSize = 4096 / sizeof(ExecutorEvent);

    /** @brief The number of events that are poped at the same time */
    constexpr static std::size_t ExecutorEventPopCount = Core::CacheLineSize / sizeof(ExecutorEvent);


    /** @brief Number of pipeline in optimized cache */
    constexpr static std::size_t OptimalPipelineCount = KUBE_ECS_PIPELINE_CACHE_COUNT;

    /** @brief Number of systems in per pipeline optimized cache */
    constexpr static std::size_t OptimalSystemPerPipelineCount = KUBE_ECS_PER_PIPELINE_SYSTEM_CACHE_COUNT;


    /** @brief Small vector optimized for pipeline storage */
    template<typename Type>
    using PipelineSmallVector = Core::SmallVector<Type, OptimalPipelineCount, ECSAllocator>;

    /** @brief Small vector optimized for system storage per pipeline */
    template<typename Type>
    using SystemSmallVector = Core::SmallVector<Type, OptimalSystemPerPipelineCount, ECSAllocator>;

    /** @brief Store the system names of a pipeline */
    using PipelineSystemNames = SystemSmallVector<Core::HashedName>;

    /** @brief Store the systems of a pipeline */
    using PipelineSystems = SystemSmallVector<SystemPtr>;

    /** @brief Store the events of a pipeline */
    using PipelineEvents = Core::UniquePtr<Core::MPSCQueue<PipelineEvent, ECSAllocator>, ECSAllocator>;

    /** @brief Store the graph of a pipeline */
    using PipelineGraph = Core::UniquePtr<Flow::Graph, ECSAllocator>;

    /** @brief Store the clock of a pipeline */
    struct alignas_quarter_cacheline PipelineClock
    {
        static constexpr std::int64_t TimeBoundMask = 1ll << 63ll;
        static constexpr std::int64_t TickRateMask = ~TimeBoundMask;

        std::int64_t maskedTickRate {};
        std::int64_t elapsed {};


        /** @brief Get tick rate from masked value */
        [[nodiscard]] inline std::int64_t tickRate(void) const noexcept { return maskedTickRate & TickRateMask; }

        /** @brief Get tick rate from masked value */
        inline void setTickRate(const std::int64_t value) noexcept { maskedTickRate = (value & TickRateMask) | (maskedTickRate & TimeBoundMask); }

        /** @brief Get time bound state from masked value */
        [[nodiscard]] inline bool isTimeBound(void) const noexcept { return maskedTickRate & TimeBoundMask; }
    };

    /** @brief Structure of array of pipelines data */
    struct alignas_double_cacheline Pipelines
    {
        PipelineSmallVector<Core::HashedName>       hashes {};
        PipelineSmallVector<PipelineEvents>         events {};
        PipelineSmallVector<PipelineSystemNames>    systemHashes {};
        PipelineSmallVector<PipelineSystems>        systems {};
        PipelineSmallVector<PipelineClock>          clocks {};
        PipelineSmallVector<PipelineGraph>          graphs {};
        PipelineSmallVector<PipelineBeginPass>      inlineBeginPasses {};
        PipelineSmallVector<PipelineBeginPass>      beginPasses {};
        PipelineSmallVector<std::string_view>       names {};
    };
    static_assert_alignof_double_cacheline(Pipelines);

    /** @brief Executor cache */
    struct alignas_double_cacheline Cache
    {
        bool running { false };
        std::int64_t lastTick {};
        std::int64_t nextTick {};
        double sleepEstimate { 5e-3 };
        double sleepMean { 5e-3 };
        double sleepM2 { 0 };
        std::int64_t sleepCount { 1 };
    };
    static_assert_fit_double_cacheline(Cache);


    /** @brief Get global instance */
    [[nodiscard]] static inline Executor &Get(void) noexcept { return *_Instance; }


    /** @brief Destructor */
    ~Executor(void) noexcept;

    /** @brief Construct scheduler with a maximum amount of workers, tasks and events */
    Executor(const std::size_t workerCount = Flow::Scheduler::AutoWorkerCount,
            const std::size_t taskQueueSize = Flow::Scheduler::DefaultTaskQueueSize,
            const std::size_t eventQueueSize = DefaultExecutorEventQueueSize) noexcept;


    /** @brief Get reference to graph scheduler */
    [[nodiscard]] inline Flow::Scheduler &scheduler(void) noexcept { return _scheduler; }
    [[nodiscard]] inline const Flow::Scheduler &scheduler(void) const noexcept { return _scheduler; }


    /** @brief Run the executor in blocking mode */
    void run(void) noexcept;

    /** @brief Tick the executor once */
    bool tick(void) noexcept;

    /** @brief Stop the executor */
    void stop(void) noexcept;


    /** @brief Add a pipeline into executor */
    template<kF::ECS::Pipeline PipelineType, PipelineTimeMode TimeMode = PipelineTimeMode::Free,
            typename BeginPass = PipelineBeginPass, typename InlineBeginPass = PipelineBeginPass>
    void addPipeline(const std::int64_t frequencyHz, const std::size_t eventQueueSize = DefaultPipelineEventQueueSize,
            BeginPass &&beginPass = PipelineBeginPass {}, InlineBeginPass &&inlineBeginPass = PipelineBeginPass {}) noexcept;

    /** @brief Alias to add pipeline without custom event queue size */
    template<kF::ECS::Pipeline PipelineType, PipelineTimeMode TimeMode = PipelineTimeMode::Free,
            typename BeginPass = PipelineBeginPass, typename InlineBeginPass = PipelineBeginPass>
    void addPipeline(const std::int64_t frequencyHz, BeginPass &&beginPass, InlineBeginPass &&inlineBeginPass = PipelineBeginPass {}) noexcept
        { addPipeline<PipelineType, TimeMode>(frequencyHz, DefaultPipelineEventQueueSize, std::forward<BeginPass>(beginPass), std::forward<InlineBeginPass>(inlineBeginPass)); }

    /** @brief Alias to add pipeline without custom event queue size and only an inline begin pass */
    template<kF::ECS::Pipeline PipelineType, PipelineTimeMode TimeMode = PipelineTimeMode::Free, typename InlineBeginPass>
    void addPipelineInline(const std::int64_t frequencyHz, InlineBeginPass &&inlineBeginPass) noexcept
        { addPipeline<PipelineType, TimeMode>(frequencyHz, DefaultPipelineEventQueueSize, PipelineBeginPass {}, std::forward<InlineBeginPass>(inlineBeginPass)); }


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
        { return _pipelines.clocks.at(pipelineIndex).tickRate(); }

    /** @brief Get pipeline tick rate from pipeline index */
    void setPipelineTickRate(const PipelineIndex pipelineIndex, const std::int64_t frequencyHz) noexcept;

    /** @brief Get pipeline time bound state from pipeline index */
    [[nodiscard]] inline bool isPipelineTimeBound(const PipelineIndex pipelineIndex) const noexcept
        { return _pipelines.clocks.at(pipelineIndex).isTimeBound(); }


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
        { return _pipelines.systems.at(pipelineIndex).at(systemIndex).get(); }


    /** @brief Send an event to a system */
    template<typename DestinationPipeline, bool RetryOnFailure = true, typename Callback>
    void sendEvent(Callback &&callback) noexcept;

    /** @brief Send an event to a system, using a specific pipeline index
     *  @note DestinationPipeline and pipelineIndex must match */
    template<bool RetryOnFailure = true, typename Callback>
    void sendEvent(const PipelineIndex pipelineIndex, Callback &&callback) noexcept;

private:
    // Global access instance
    static Executor *_Instance;

    // Cacheline 0 -> 13
    Flow::Scheduler _scheduler {};

    // Cacheline 14 -> 17
    Core::MPSCQueue<ExecutorEvent, ECSAllocator> _eventQueue;

    // Cacheline 18 -> 19
    Cache _cache {};

    // Cacheline 20 -> ... (depend on 'OptimalPipelineCount')
    Pipelines _pipelines {};

    /** @brief Process executor events */
    [[nodiscard]] bool processEvents(void) noexcept;

    /** @brief Observe executor pipelines */
    void observePipelines(void) noexcept;

    /** @brief Wait executor pipelines */
    void waitPipelines(void) noexcept;


    /** @brief Build all pipeline graphs */
    void buildPipelineGraphs(void) noexcept;

    /** @brief Build a pipeline graph given its index */
    void buildPipelineGraph(const PipelineIndex pipelineIndex) noexcept;

    /** @brief Wait executor to enter IDLE state (no pipeline running) */
    void waitIDLE(void) noexcept;
};
static_assert_alignof_double_cacheline(kF::ECS::Executor);

#include "Executor.ipp"