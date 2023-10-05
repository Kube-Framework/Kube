/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow graph
 */

#pragma once

#include "Task.hpp"

namespace kF::Flow
{
    class Graph;

    /** @brief Unique pointer over Graph */
    using GraphPtr = Core::UniquePtr<Graph, FlowAllocator>;
}

/** @brief Task based graph */
class alignas_cacheline kF::Flow::Graph
{
public:
    /** @brief The default wait sleep duration in nanoseconds */
    static constexpr std::size_t DefaultWaitSleepTime = 10'000'000;


    /** @brief Destructor */
    ~Graph(void) noexcept;

    /** @brief Default constructor */
    inline Graph(void) noexcept = default;


    /** @brief Check if graph is running */
    [[nodiscard]] inline bool running(void) const noexcept { return _activeTaskCount.load(std::memory_order_acquire); }


    /** @brief Add task into graph with a work functor */
    template<typename WorkFunc>
        // requires std::constructible_from<kF::Flow::Task::Work, WorkFunc>
    Task &add(WorkFunc &&work) noexcept;

    /** @brief Add task into graph with a free function */
    template<auto Function>
        // requires std::constructible_from<kF::Flow::Task::Work, decltype(Function)>
    Task &add(void) noexcept;

    /** @brief Add task into graph with a member function */
    template<auto MemberFunction, typename ClassType>
    Task &add(ClassType &&instance) noexcept;


    /** @brief Remove a task */
    void remove(Task &task) noexcept;


    /** @brief Remove all tasks in graph */
    void clear(void) noexcept;


    /** @brief Get task count */
    [[nodiscard]] inline std::uint32_t count(void) const noexcept { return _tasks.size(); }


    /** @brief Get last execution time in nanoseconds */
    [[nodiscard]] inline std::int64_t lastExecutionTime(void) const noexcept { return _lastExecutionTime; }

    /** @brief Get median execution time in nanoseconds */
    [[nodiscard]] inline std::int64_t medianExecutionTime(void) const noexcept { return _medianExecutionTime; }


    /** @brief Wait until graph execution is over
     *  Implementation uses a sleep loop, great for energy efficiency
     *  @param sleepNs The amount of nanoseconds per sleep */
    void waitSleep(const std::int64_t sleepNs = DefaultWaitSleepTime) noexcept;

    /** @brief Wait until a specific time point is reached
     *  @param timestamp The wake-up timestamp in nanoseconds */
    void waitUntil(const std::int64_t timestamp) noexcept;


    /** @brief Wait until graph execution is over
     *  Implementation uses a spin loop, great for critical section wait
     *  Please note that waitSleep is much more energy-efficient */
    void waitSpin(void) noexcept;


public: // Unsafe public functions reserved for workers
    /**
     * @brief Prepare current graph to schedule setting its state to running
     * ! Only call this function before scheduling the graph
     * @return TaskRefList The list of tasks to schedule
     */
    [[nodiscard]] const TaskRefList &prepareToSchedule(void) noexcept;


    /** @brief Decrement counter of active tasks by taskCount */
    void joinTasks(const std::size_t taskCount) noexcept;


private:
    // Cacheline 0
    TaskList _tasks {}; // Children task instances
    TaskRefList _preparedTasks {}; // Tasks prepared to run
    std::atomic_size_t _activeTaskCount { 0 }; // Number of active tasks
    std::int64_t _beginExecutionTimestamp {}; // Timestamp of execution begin
    std::int64_t _lastExecutionTime {}; // Last execution time
    std::int64_t _medianExecutionTime {}; // Median execution time


    /** @brief Invalidate schedule cache so it must be re-prepared */
    inline void invalidateScheduleCache(void) noexcept { if (!_preparedTasks.empty()) [[unlikely]] invalidateScheduleCacheImpl(); }

    /** @brief This function extends invalidateScheduleCache in order to reduce inline footprint on the likely path */
    void invalidateScheduleCacheImpl(void) noexcept;
};

static_assert_fit_cacheline(kF::Flow::Graph);

#include "Graph.ipp"