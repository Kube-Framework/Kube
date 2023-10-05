/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow Scheduler
 */


#include <Kube/Core/Abort.hpp>
#include <Kube/Core/Random.hpp>

#include "Scheduler.hpp"
#include "Graph.hpp"

using namespace kF;

Flow::Scheduler::~Scheduler(void) noexcept
{
    _running.store(false, std::memory_order_relaxed);
    _notifier.release(static_cast<ptrdiff_t>(workerCount())); // Release all sleeping workers (Scheduler enter into non-reusable state)
    for (auto &thd : _threads)
        thd.join();
}

Flow::Scheduler::Scheduler(const std::size_t workerCount, const std::size_t taskQueueSize) noexcept
    : _taskQueue(taskQueueSize)
{
    // Find worker count
    auto count = workerCount;
    if (count == AutoWorkerCount)
        count = std::thread::hardware_concurrency();
    if (!count)
        count = DefaultWorkerCount;

    _workers.allocate(static_cast<std::uint32_t>(count), taskQueueSize);
    _threads.allocate(static_cast<std::uint32_t>(count));

    std::uint32_t workerIndex { 0u };
    for (auto &thd : _threads) {
        thd = std::thread([this, workerIndex] {
            runWorker(workerIndex);
        });
        ++workerIndex;
    }
}

void Flow::Scheduler::schedule(Graph &graph) noexcept
{
    auto &tasks = graph.prepareToSchedule();
    // Ensure the graph is not empty before scheduling
    if (!tasks.empty()) [[likely]] {
        for (const auto task : tasks) {
            while (!_taskQueue.push(task))
                std::this_thread::yield();
        }
        notifyWorker();
    }
}

void Flow::Scheduler::schedule(Task &task) noexcept
{
    while (!_taskQueue.push(&task))
        std::this_thread::yield();
    notifyWorker();
}

void Flow::Scheduler::runWorker(const std::uint32_t workerIndex) noexcept
{
    WorkerCache cache {
        .index = workerIndex,
        .queue = _workers[workerIndex]
    };

    while (true) {
        // If a task is found, start execution
        if (cache.task != nullptr) [[likely]] {
            // Set this worker in active mode and notify if this is the only one running
            if (_activeWorkerCount.fetch_add(1, std::memory_order_acq_rel) == 0
                    && _stealWorkerCount.load(std::memory_order_acquire) == 0) [[unlikely]]
                notifyWorker();
            // Execute worker queue
            executeWorkerQueue(cache);
            // Leave active mode
            _activeWorkerCount.fetch_sub(1, std::memory_order_acq_rel);

        // No more task to execute
        } else if (!waitWorkerTask(cache)) [[unlikely]] {
            break;
        }
    }
}

bool Flow::Scheduler::observeWorkerPendingGraphs(WorkerCache &cache) noexcept
{
    // Iterate over each graph, select completed ones
    bool hasScheduled = false;
    auto it = std::remove_if(cache.pendingGraphs.begin(), cache.pendingGraphs.end(),
        [this, &cache, &hasScheduled](PendingGraph &pending) {
            // If the graph is running, do nothing
            if (pending.graph->running()) [[likely]]
                return false;

            // Notify end of task
            pending.task->parent()->joinTasks(1u);

            // Schedule linked tasks
            if (auto &linkedTo = pending.task->linkedTo(); !linkedTo.empty()) [[likely]] {
                hasScheduled = true;
                scheduleWorkerLinkedTasks(cache, linkedTo.begin(), linkedTo.end());
            }
            return true;
        }
    );
    // Remove completed graphs
    if (it != cache.pendingGraphs.end()) [[unlikely]]
        cache.pendingGraphs.erase(it, cache.pendingGraphs.end());
    return hasScheduled;
}

void Flow::Scheduler::executeWorkerQueue(WorkerCache &cache) noexcept
{
    while (true) {
        // Execute if this worker is the only running
        if (cache.task) [[likely]]
            executeWorkerTask(cache);
        if (!cache.pendingGraphs.empty()) [[unlikely]]
            observeWorkerPendingGraphs(cache);
        if (!cache.queue.pop(cache.task)) [[unlikely]] // No more task in queue, stop execution
            break;
    }
}

void Flow::Scheduler::executeWorkerTask(WorkerCache &cache) noexcept
{
    // Remove the task from the cache
    auto task = cache.task;
    cache.task = nullptr;
    std::size_t switchIndex { SIZE_MAX };

    // Execute task
    switch (auto &work = task->work(); work.type) {
    case Task::WorkType::Static:
        work.staticWork();
        break;
    case Task::WorkType::Switch:
        switchIndex = work.switchWork();
        break;
    case Task::WorkType::Graph:
        // If the graph contains tasks, add it to pending list and setup task range
        if (auto &tasks = work.graphWork->prepareToSchedule(); !tasks.empty()) [[likely]] {
            scheduleWorkerTasks(cache, tasks.begin(), tasks.end());
            cache.pendingGraphs.push(PendingGraph { .task = task, .graph = work.graphWork });
            return;
        }
        break;
    default:
        break;
    }

    // Ensure switchIndex is valid given linked tasks
    auto &linkedTo = task->linkedTo();
    kFEnsure(switchIndex == SIZE_MAX || switchIndex <= linkedTo.size(),
        "Flow::Scheduler::executeWorkerTask: Task returned switch index '", switchIndex,
        "' but only has '", linkedTo.size(), "' linked tasks");

    // Setup scheduled tasks and joined tasks
    std::size_t joinedTasks { 1ul };
    Task * const *begin {};
    Task * const *end {};
    if (switchIndex == SIZE_MAX) [[likely]] {
        begin = linkedTo.begin();
        end = linkedTo.end();
    } else {
        // Join all tasks discarded by the condition
        for (std::size_t i = 0ul; const auto link : linkedTo) {
            if (i++ != switchIndex) [[likely]]
                joinWorkerConditionalTask(cache, link, joinedTasks);
        }
        // If 'switchIndex' is equal to the count of task, we must not schedule any
        if (switchIndex != linkedTo.size()) [[likely]] {
            begin = linkedTo.begin() + switchIndex;
            end = begin + 1;
        }
    }

    // Notify end of tasks
    if (const auto parent = task->parent(); parent)
        parent->joinTasks(joinedTasks);

    // Schedule target linked tasks
    if (begin != end) [[likely]]
        scheduleWorkerLinkedTasks(cache, begin, end);
}

void Flow::Scheduler::joinWorkerConditionalTask(WorkerCache &cache, Task * const task, std::size_t &joinCount) noexcept
{
    // If a task couldn't be joined, it means the task is connected to other sources
    if (!task->tryJoin()) [[likely]]
        return;
    ++joinCount;
    for (const auto link : task->linkedTo())
        joinWorkerConditionalTask(cache, link, joinCount);
}

void Flow::Scheduler::scheduleWorkerTasks(WorkerCache &cache, Task * const *begin, Task * const *end) noexcept
{
    while (true) {
        // Insert as much as possible into worker queue
        begin += cache.queue.pushRange(begin, end);
        if (begin == end) [[likely]]
            break;

        // Insert as much as possible into global queue
        while (begin != end && _taskQueue.push(*begin)) [[likely]]
            ++begin;
        if (begin == end) [[likely]]
            break;

        // The scenario is going suboptimal here, we execute the local queue recursively and loop again
        executeWorkerQueue(cache);
    }
}

void Flow::Scheduler::scheduleWorkerLinkedTasks(WorkerCache &cache, Task * const *begin, Task * const *end) noexcept
{
    // Schedule linked tasks
    auto it = begin;
    while (true) {
        // Try to join linked task
        if (it != end && (*it)->tryJoin()) [[likely]] {
            ++it;
            continue;
        // We either reached the end of the list or an unjoinable task
        } else [[unlikely]] {
            scheduleWorkerTasks(cache, begin, it);
            if (it == end) [[likely]]
                break;
            begin = ++it;
        }
    }
}

bool Flow::Scheduler::waitWorkerTask(WorkerCache &cache) noexcept
{
    const auto onTaskFound = [this] {
        if (_stealWorkerCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            notifyWorker();
        return true;
    };

    while (true) {
        // Set this worker in steal mode
        _stealWorkerCount.fetch_add(1, std::memory_order_acq_rel);

        while (true) {
            // Try to steal a task
            if (stealWorkerTask(cache))
                return onTaskFound();

            // Try to steal a task from scheduler queue
            if (_taskQueue.size()) {
                // We know that _taskQueue can still be inserting so we try again if pop fails
                if (_taskQueue.pop(cache.task))
                    return onTaskFound();
            // If the taskqueue size is 0 we stop only if we don't have pending graphs
            } else if (cache.pendingGraphs.empty())
                break;
        }

        // Leave steal mode, if there are no other thief but still active workers, prevent sleeping
        if (_stealWorkerCount.fetch_sub(1, std::memory_order_acq_rel) == 1
                && _activeWorkerCount.load(std::memory_order_acquire) > 0)
            continue;
        else
            break;
    }

    // If scheduler is still running, start sleeping
    if (_running.load(std::memory_order_relaxed)) [[likely]] {
        sleepWorker();
        return _running.load(std::memory_order_relaxed);
    } else
        return false;
}

bool Flow::Scheduler::stealWorkerTask(WorkerCache &cache) noexcept
{
    const auto workerCount = _workers.size();
    const auto stealBound = StealBoundRatio * (workerCount + 1);

    std::size_t stealFailCount { 0u };
    std::size_t yieldCount { 0u };

    while (_running.load(std::memory_order_relaxed)) {
        // Observe pending graphs if any, return if one completed
        if (!cache.pendingGraphs.empty() && observeWorkerPendingGraphs(cache)) [[unlikely]]
            return true;
        // Generate a random index [0, workerCount[
        const auto targetIndex = Core::Random::Generate32(workerCount);
        // If the index is 'this' worker index, try to pop from scheduler queue
        if (targetIndex == cache.index) {
            if (_workers[targetIndex].pop(cache.task))
                break;
        // Else try to steal from target worker
        } else {
            if (_taskQueue.pop(cache.task))
                break;
        }
        // Ensure we don't steal forever
        if (++stealFailCount >= stealBound) {
            std::this_thread::yield();
            if (++yieldCount == YieldBound)
                break;
        }
    }
    return cache.task != nullptr;
}

void Flow::Scheduler::sleepWorker(void) noexcept
{
    _notifier.acquire();
}

void Flow::Scheduler::notifyWorker(void) noexcept
{
    _notifier.release();
}