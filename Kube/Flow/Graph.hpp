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
    /** @brief Destructor */
    ~Graph(void) noexcept;

    /** @brief Default constructor */
    inline Graph(void) noexcept = default;


    /** @brief Check if graph is running */
    [[nodiscard]] inline bool running(void) const noexcept { return _running.load(std::memory_order_acquire); }


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


    /** @brief Wait until graph execution is over */
    void wait(void) noexcept;


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
    /** @brief Invalidate schedule cache so it must be re-prepared */
    inline void invalidateScheduleCache(void) noexcept { if (!_preparedTasks.empty()) [[unlikely]] invalidateScheduleCacheImpl(); }

    /** @brief This function extends invalidateScheduleCache in order to reduce inline footprint on the likely path */
    void invalidateScheduleCacheImpl(void) noexcept;


    // Cacheline 0
    TaskList _tasks {}; // Children task instances
    TaskRefList _preparedTasks {}; // Tasks prepared to run
    std::atomic_size_t _activeTaskCount { 0 }; // Number of active tasks
    std::atomic_bool _running {}; // Is graph running ?
};

static_assert_fit_cacheline(kF::Flow::Graph);

#include "Graph.ipp"