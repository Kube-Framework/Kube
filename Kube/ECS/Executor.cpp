/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: System Scheduler
 */

#include <chrono>
#include <limits>
#include <thread>
#include <cmath>

#include <Kube/Core/Abort.hpp>

#include "Executor.hpp"

using namespace kF;

ECS::Executor *ECS::Executor::_Instance {};

ECS::Executor::~Executor(void) noexcept
{
    kFEnsure(!_cache.running,
        "Executor::~Executor: Executor destroyed while still running");
    kFEnsure(_Instance,
        "ECS::Executor: Executor already destroyed");
    _Instance = nullptr;
}

ECS::Executor::Executor(const std::size_t workerCount, const std::size_t taskQueueSize, const std::size_t eventQueueSize) noexcept
    : _scheduler(workerCount, taskQueueSize)
    , _eventQueue(eventQueueSize, false)
{
    kFEnsure(!_Instance,
        "ECS::Executor: Executor can only be instantiated once");
    _Instance = this;
}

Core::Expected<ECS::PipelineIndex> ECS::Executor::getPipelineIndex(const Core::HashedName pipelineHash) const noexcept
{
    Core::Expected<PipelineIndex> res;

    for (PipelineIndex index = 0u; const auto hash : _pipelines.hashes) {
        if (hash != pipelineHash) [[likely]] {
            ++index;
            continue;
        } else [[unlikely]] {
            res = Core::Expected<PipelineIndex>(index);
            break;
        }
    }
    return res;
}

Core::Expected<ECS::PipelineIndex> ECS::Executor::getSystemIndex(const PipelineIndex pipelineIndex, const Core::HashedName systemHash) const noexcept
{
    Core::Expected<PipelineIndex> res;

    // Find system inside pipeline
    auto &names = _pipelines.systemHashes.at(pipelineIndex);
    for (PipelineIndex index = 0; const auto name : names) {
        if (name != systemHash) [[likely]] {
            ++index;
            continue;
        } else [[unlikely]] {
            res = Core::Expected<PipelineIndex>(index);
            break;
        }
    }
    return res;
}

void ECS::Executor::setPipelineTickRate(const PipelineIndex pipelineIndex, const std::int64_t frequencyHz) noexcept
{
    kFEnsure(frequencyHz >= 0, "ECS::Executor::addPipeline: Pipeline only support frequency in range [0, inf[");
    ExecutorEvent event([this, pipelineIndex, frequencyHz = std::int32_t(frequencyHz)] {
        _pipelines.clocks.at(pipelineIndex).setTickRate(HzToRate(std::int64_t(frequencyHz)));
        return true;
    });
    while (!_eventQueue.push<true>(event))
        std::this_thread::yield();
}

void ECS::Executor::stop(void) noexcept
{
    ExecutorEvent event([this] {
        // Wait all pipelines to stop
        waitIdle();
        // Exhaust pipeline events before closing
        for (auto &queue : _pipelines.events) {
            PipelineEvent event;
            while (queue->pop(event))
                event();
        }
        return false;
    });
    while (!_eventQueue.push<true>(event))
        std::this_thread::yield();
}

void ECS::Executor::run(void) noexcept
{
    // Setup
    _cache.running = true;
    _cache.lastTick = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    buildPipelineGraphs();

    // Run until executor receive stop event
    while (tick());

    // Stop
    _cache.running = false;
}

bool ECS::Executor::tick(void) noexcept
{
    // Observe pipelines
    observePipelines();

    // Process event & quit if necessary
    if (!processEvents()) [[unlikely]]
        return false;

    // Wait next pipeline tick
    waitPipelines();
    return true;
}

bool ECS::Executor::processEvents(void) noexcept
{
    ExecutorEvent event;
    while (_eventQueue.pop(event)) {
        // If event returned false, stop executor
        if (!event()) [[unlikely]]
            return false;
    }
    return true;
}

void ECS::Executor::observePipelines(void) noexcept
{
    const std::int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const std::int64_t elapsed = now - _cache.lastTick;
    std::int64_t next = INT64_MAX;

    // kFInfo("[ECS Executor] Observe pipelines ", elapsed);
    // Iterate over each pipeline clock
    for (PipelineIndex pipelineIndex = 0; auto &clock : _pipelines.clocks) {
        const auto tickRate = clock.tickRate(); // @todo fix this datarace when clock frequency changes at runtime (mutex / atomic)
        const bool isTimeBound = clock.isTimeBound();
        clock.elapsed += elapsed;
        // If the pipeline has to be executed
        if (clock.elapsed >= tickRate) [[unlikely]] {
            // If the graph is not being executed, schedule it
            if (auto &graph = *_pipelines.graphs.at(pipelineIndex); !graph.running()) [[likely]] {
                // kFInfo("[ECS Executor] | Tick '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
                if (isTimeBound)
                    clock.elapsed -= tickRate;
                else
                    clock.elapsed = 0;
                if (const auto &inlineBeginPass = _pipelines.inlineBeginPasses.at(pipelineIndex); !inlineBeginPass || inlineBeginPass()) [[likely]]
                    _scheduler.schedule(graph);
                next = std::min(next, now + tickRate);
            // Else we must schedule the graph as soon as it finishes execution
            } else {
                // kFInfo("[ECS Executor] | Busy '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
                next = now;
            }
        // The pipeline have time before schedule
        } else {
            // kFInfo("[ECS Executor] | Wait '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
            next = std::min(next, now + tickRate - clock.elapsed);
        }
        ++pipelineIndex;
    }
    _cache.lastTick = now;
    _cache.nextTick = next;
}

void ECS::Executor::waitPipelines(void) noexcept
{
    while (true) {
        const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        const std::int64_t nanoseconds = _cache.nextTick - now;
        // Don't sleep, we are late
        if (nanoseconds <= 0)
            break;
        // Sleep for the whole duration if we have enough time to sleep
        else if (nanoseconds > 500'000) {
            Flow::PreciseSleep(nanoseconds);
            // const auto end = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            // const auto observed = end - now;
            // if (end > _cache.nextTick + 100'000)
            //     kFInfo("[ECS Executor] Oversleep ", observed / 1e6, " / ", nanoseconds / 1e6, "ms ", (double(observed) / double(nanoseconds)) * 100.0, "%");
        // Spin wait
        } else
            std::this_thread::yield();
    }
}

void ECS::Executor::buildPipelineGraphs(void) noexcept
{
    for (PipelineIndex index {}, count = _pipelines.hashes.size(); index != count; ++index) {
        buildPipelineGraph(index);
    }
}

void ECS::Executor::buildPipelineGraph(const PipelineIndex pipelineIndex) noexcept
{
    // Retreive pipeline system list & graph
    auto &systems = _pipelines.systems.at(pipelineIndex);
    auto &graph = *_pipelines.graphs.at(pipelineIndex);

    // Clear old graph
    graph.clear();

    // The first taks of the pipeline executes events and tells if the pipeline can execute
    auto &beginTask = graph.add([this, pipelineIndex] {
        // Process all pipeline events
        auto &queue = *_pipelines.events.at(pipelineIndex);
        PipelineEvent event;
        while (queue.pop(event))
            event();

        // If we have a begin pass function, call it to know if the pipeline must execute
        if (auto &beginPass = _pipelines.beginPasses.at(pipelineIndex); !beginPass)
            return false;
        else
            return !beginPass();
    });

    // If pipeline does not contain any system, don't build further
    if (systems.empty()) [[unlikely]]
        return;

    Flow::Task *prevTickTask = &beginTask;
    Flow::Task *prevGraphTask = nullptr;

    // For each system, record tick & graph tasks then link them to begin / end
    for (auto &system : systems) {
        auto &tickTask = graph.add([system = system.get()](void) -> bool { return !system->tick(); });
        auto &graphTask = graph.add(&system->taskGraph());

        // Connect tick to previous tasks
        tickTask.after(*prevTickTask);
        if (prevGraphTask) [[likely]]
            tickTask.after(*prevGraphTask);
        // Connect tick to its graph
        tickTask.before(graphTask);

        // Set previous tasks
        prevTickTask = &tickTask;
        prevGraphTask = &graphTask;
    }
}

void ECS::Executor::waitIdle(void) noexcept
{
    for (auto &graph : _pipelines.graphs)
        graph->waitSpin();
}
