/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow graph
 */

#include <thread>

#include <Kube/Core/Abort.hpp>

#include "Graph.hpp"

using namespace kF;

Flow::Graph::~Graph(void) noexcept
{
    wait();
}

const Flow::TaskRefList &Flow::Graph::prepareToSchedule(void) noexcept
{
    // Ensure that the graph is not already running
    kFEnsure(!_running.load(std::memory_order_acquire), "Flow::Graph::prepareToSchedule: Graph is already running");

    // Set graph running state & active task count
    _running.store(true, std::memory_order_release);
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
    return _preparedTasks;
}

void Flow::Graph::joinTasks(const std::size_t taskCount) noexcept
{
    // The last worker is ending graph execution
    if (_activeTaskCount.fetch_sub(taskCount, std::memory_order_acq_rel) == taskCount) [[unlikely]] {
        _running.store(false, std::memory_order_release);
        _running.notify_all();
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

void Flow::Graph::wait(void) noexcept
{
    _running.wait(true);
}
