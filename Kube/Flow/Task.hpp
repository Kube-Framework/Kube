/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow task
 */

#pragma once

#include <Kube/Core/Vector.hpp>
#include <Kube/Core/Functor.hpp>
#include <Kube/Core/UniquePtr.hpp>

#include "Base.hpp"

namespace kF::Flow
{
    class Task;
    class Graph;

    /** @brief Unique pointer over Task */
    using TaskPtr = Core::UniquePtr<Task, FlowAllocator>;

    /** @brief List of tasks */
    using TaskList = Core::Vector<TaskPtr, FlowAllocator>;

    /** @brief List of task references */
    using TaskRefList = Core::Vector<Task *, FlowAllocator>;
}

/** @brief Task in a graph */
class alignas_double_cacheline kF::Flow::Task
{
public:
    /** @brief Empty work structure used for safe initialization */
    struct NoneWork {};

    /** @brief Static work */
    using StaticWork = Core::Functor<void(void), FlowAllocator, Core::CacheLineEighthSize * 7>;

    /** @brief Switch work, can use any numeric as return type as long as it belongs inside range [0, TaskCount]
     *  @note Index TaskCount (out of the task indexing range) means no task is selected by the switch (like a 'default: break') */
    using SwitchWork = Core::Functor<std::size_t(void), FlowAllocator, Core::CacheLineEighthSize * 7>;

    /** @brief Graph work */
    using GraphWork = Graph *;

    /** @brief Types of work */
    enum class WorkType : std::uint32_t
    {
        None,
        Static,
        Switch,
        Graph
    };

    /** @brief Work */
    struct alignas_cacheline Work
    {
        WorkType type { WorkType::None };
        union {
            NoneWork noneWork {};
            StaticWork staticWork;
            SwitchWork switchWork;
            GraphWork graphWork;
        };

        /** @brief Destructor */
        ~Work(void) noexcept;

        /** @brief Work constructor */
        template<typename Type>
        Work(Type &&value) noexcept;
    };
    static_assert_fit_cacheline(Work);


    /** @brief Destructor */
    ~Task(void) noexcept;

    /** @brief Task is not copiable */
    Task(const Task &other) noexcept = delete;
    Task &operator=(const Task &other) noexcept = delete;

    /** @brief Constructor */
    template<typename WorkFunc>
    inline Task(WorkFunc &&work) noexcept
        : _work(std::forward<WorkFunc>(work)) {}

    /** @brief Graph constructor */
    template<typename WorkFunc>
    inline Task(Graph &parent, WorkFunc &&work) noexcept
        : _work(std::forward<WorkFunc>(work)), _parent(&parent) {}


    /** @brief Parent graph getter */
    [[nodiscard]] inline Graph *parent(void) const noexcept { return _parent; }


    /** @brief Get opaque work functor instance */
    [[nodiscard]] inline Work &work(void) noexcept { return _work; }

    /** @brief Get opaque work functor constant instance */
    [[nodiscard]] inline const Work &work(void) const noexcept { return _work; }


    /** @brief Get tasks linked before this node */
    [[nodiscard]] inline const TaskRefList &linkedFrom(void) const noexcept { return _linkedFrom; }

    /** @brief Get tasks linked after this node */
    [[nodiscard]] inline const TaskRefList &linkedTo(void) const noexcept { return _linkedTo; }


    /** @brief Setup task so it executes before 'other' */
    Task &before(Task &other) noexcept;

    /** @brief Setup task so it executes after 'other' */
    Task &after(Task &other) noexcept;


    /** @brief Reset before & after links */
    void reset(void) noexcept;

public: // Unsafe public functions reserved for workers
    /** @brief Try to join the task
     *  @return true The task is ready to get executed
     *  @return false The task still have unexecuted dependencies */
    [[nodiscard]] inline bool tryJoin(void) noexcept
        { return ++_joinCount == _linkedFrom.size(); }

    /** @brief Reset the join count of the task */
    inline void prepareToSchedule(void) noexcept { _joinCount.store(0, std::memory_order_relaxed); }


private:
    // Cacheline 0
    Work _work;

    // Cacheline 1
    Graph *_parent {};
    std::atomic_size_t _joinCount {};
    TaskRefList _linkedFrom {};
    TaskRefList _linkedTo {};
};

static_assert_fit_double_cacheline(kF::Flow::Task);

#include "Task.ipp"