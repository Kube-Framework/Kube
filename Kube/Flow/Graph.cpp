/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow graph
 */

#include <chrono>
#include <thread>

#include <Kube/Core/Abort.hpp>

#include "Graph.hpp"

using namespace kF;

Flow::Graph::~Graph(void) noexcept
{
    waitSleep();
}

const Flow::TaskRefList &Flow::Graph::prepareToSchedule(void) noexcept
{
    { // Ensure that the graph is not running
        const auto count = _activeTaskCount.load(std::memory_order_acquire);
        kFEnsure(count == 0,
            "Flow::Graph::prepareToSchedule: Graph is already running with ", count, " active tasks");
    }

    _activeTaskCount.store(_tasks.size(), std::memory_order_relaxed);
    if (!_preparedTasks.empty()) [[likely]] {
        for (auto &task : _tasks)
            task->prepareToSchedule();
    } else {
        for (auto &task : _tasks) {
            task->prepareToSchedule();
            if (!task->linkedFrom().empty()) [[likely]]
                continue;
            else
                _preparedTasks.push(task.get());
        }
    }
    _beginExecutionTimestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return _preparedTasks;
}

void Flow::Graph::joinTasks(const std::size_t taskCount) noexcept
{
    // The last worker is ending graph execution
    if (_activeTaskCount.fetch_sub(taskCount, std::memory_order_acq_rel) == taskCount) [[unlikely]] {
        _lastExecutionTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - _beginExecutionTimestamp;
        _medianExecutionTime = (_medianExecutionTime + _lastExecutionTime) / 2;
    }
}

void Flow::Graph::remove(Task &task) noexcept
{
    invalidateScheduleCache();
    const auto it = _tasks.find(&task);
    kFEnsure(it != _tasks.end(), "Flow::Graph::remove: Task doesn't exists");
    task.reset();
    _tasks.erase(it);
}

void Flow::Graph::clear(void) noexcept
{
    invalidateScheduleCache();
    _tasks.clear();
}

void Flow::Graph::invalidateScheduleCacheImpl(void) noexcept
{
    _preparedTasks.clear();
}

void Flow::Graph::waitSleep(const std::int64_t sleepNs) noexcept
{
    using namespace std::chrono;

    while (_activeTaskCount.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(nanoseconds(sleepNs));
    }
}

void Flow::Graph::waitSpin(void) noexcept
{
    while (_activeTaskCount.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
}
