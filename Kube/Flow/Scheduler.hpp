/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow Scheduler
 */

#pragma once

#include <semaphore>
#include <future>
#include <thread>

#include <Kube/Core/SmallVector.hpp>
#include <Kube/Core/MPMCQueue.hpp>
#include <Kube/Core/SPMCQueue.hpp>
#include <Kube/Core/HeapArray.hpp>

#include "Base.hpp"

namespace kF::Flow
{
    class Scheduler;
    class Task;
    class Graph;
}

class alignas_double_cacheline kF::Flow::Scheduler
{
public:
    /** @brief Use this variable to automatically detect hardware thread count */
    static constexpr std::size_t AutoWorkerCount { 0ul };

    /** @brief This variable is used on hardware thread detection failure */
    static constexpr std::size_t DefaultWorkerCount { 4ul };

    /** @brief Default queue size of tasks */
    static constexpr std::size_t DefaultTaskQueueSize { 512ul };


    /** @brief Ratio required to compute the steal bound
     *  StealBound = StealBoundRatio * (WorkerCount + 1) */
    static constexpr std::size_t StealBoundRatio = 2;

    /** @brief Maximum number of yields before sleeping */
    static constexpr std::size_t YieldBound = 100;


    /** @brief Worker queue */
    using WorkerQueue = Core::SPMCQueue<Task *, FlowAllocator>;

    /** @brief Pending uncompleted graph */
    struct PendingGraph
    {
        Task *task {};
        Graph *graph {};
    };

    /** @brief Worker cache, only accessible from one worker */
    struct alignas_double_cacheline WorkerCache
    {
        alignas(8) std::uint32_t index {};
        WorkerQueue &queue;
        Task *task {};
        Core::SmallVector<PendingGraph, 5, FlowAllocator> pendingGraphs {};
    };
    static_assert_fit_double_cacheline(WorkerCache);


    /** @brief Destroy and join all workers */
    ~Scheduler(void) noexcept;

    /** @brief Constructor setup scheduler and its workers */
    Scheduler(const std::size_t workerCount = AutoWorkerCount, const std::size_t taskQueueSize = DefaultTaskQueueSize) noexcept;


    /** @brief Get worker counter */
    [[nodiscard]] inline std::size_t workerCount(void) const noexcept { return static_cast<std::size_t>(_workers.size()); }


    /** @brief Schedule execution of a graph */
    void schedule(Graph &graph) noexcept;

    /** @brief Schedule execution of a task
     *  @note The task must be valid the during whole duration of it processing */
    void schedule(Task &task) noexcept;


private:
    /** @brief Run worker in blocking mode (must be called inside a worker thread) */
    void runWorker(const std::uint32_t workerIndex) noexcept;

    /** @brief Observe all pending graphs of a worker */
    bool observeWorkerPendingGraphs(WorkerCache &cache) noexcept;

    /** @brief Execute worker queue */
    void executeWorkerQueue(WorkerCache &cache) noexcept;


    /** @brief Execute a task and set next one if any */
    void executeWorkerTask(WorkerCache &cache) noexcept;

    /** @brief Join a task ignored from a conditional flow */
    void joinWorkerConditionalTask(WorkerCache &cache, Task * const task, std::size_t &joinCount) noexcept;

    /** @brief Schedule a range of tasks from worker perspective */
    void scheduleWorkerTasks(WorkerCache &cache, Task * const *begin, Task * const *end) noexcept;

    /** @brief Schedule a range of linked tasks from worker perspective */
    void scheduleWorkerLinkedTasks(WorkerCache &cache, Task * const *begin, Task * const *end) noexcept;


    /** @brief Make worker wait until a task is ready */
    [[nodiscard]] bool waitWorkerTask(WorkerCache &cache) noexcept;


    /** @brief Try to steal a task for worker to continue executing */
    [[nodiscard]] bool stealWorkerTask(WorkerCache &cache) noexcept;


    /** @brief Put worker onto sleep until notified (must be called from a worker thread) */
    void sleepWorker(void) noexcept;


    /** @brief Wake up a single worker (can be called on any thread) */
    void notifyWorker(void) noexcept;


    // Cacheline 0 -> 3
    Core::MPMCQueue<Task *, FlowAllocator> _taskQueue;

    // Cacheline 4 & 5
    Core::HeapArray<WorkerQueue, FlowAllocator> _workers {}; // Worker array
    Core::HeapArray<std::thread> _threads {}; // We don't use FlowAllocator because threads are permanently unaccessed until destruction

    // Cacheline 6 & 7
    alignas_double_cacheline std::atomic_bool _running { true };

    // Cacheline 8 & 9
    alignas_double_cacheline std::counting_semaphore<> _notifier { 0 };

    // Cacheline 10 & 11
    alignas_double_cacheline std::atomic_size_t _activeWorkerCount { 0 };

    // Cacheline 12 & 13
    alignas_double_cacheline std::atomic_size_t _stealWorkerCount { 0 };
};

static_assert_alignof_double_cacheline(kF::Flow::Scheduler);
static_assert_sizeof(kF::Flow::Scheduler, kF::Core::CacheLineDoubleSize * 7);