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
    kFEnsure(!_running,
        "Executor::~Executor: Executor destroyed while still running");
    kFEnsure(_Instance,
        "ECS::Executor: Executor already destroyed");
    _Instance = nullptr;
}

ECS::Executor::Executor(const std::size_t workerCount, const std::size_t taskQueueSize) noexcept
    : _scheduler(workerCount, taskQueueSize)
{
    kFEnsure(!_Instance,
        "ECS::Executor: Executor can only be instantiated once");
    _Instance = this;
}

Core::Expected<ECS::PipelineIndex> ECS::Executor::getPipelineIndex(const Core::HashedName pipelineHash) const noexcept
{
    Core::Expected<PipelineIndex> res;

    for (PipelineIndex index = 0u; const auto hash : _pipelineHashes) {
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
    auto &names = _pipelines.at(pipelineIndex)->systemHashes;
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

void ECS::Executor::run(const Core::HashedName mainThreadPipelineHash, Core::TrivialFunctor<void(void)> &&mainThreadInlineTick) noexcept
{
    kFEnsure(!_pipelines.empty(), "ECS::Executor::run: No pipeline registered");

    // Setup
    _running = true;

    for (PipelineIndex index {}, count = _pipelineHashes.size(); index != count; ++index)
        buildPipelineGraph(*_pipelines[index]);

    // Find main pipeline
    const auto mainPipeline = [this, mainThreadPipelineHash] {
        auto mainPipeline = _pipelineHashes.find(mainThreadPipelineHash);
        if (mainPipeline == _pipelineHashes.end())
            mainPipeline = _pipelineHashes.begin();
        return _pipelines.begin() + Core::Distance<std::size_t>(_pipelineHashes.begin(), mainPipeline);
    }();

    // Run pipelines from 2nd to last in other threads
    for (auto pipelineIt = _pipelines.begin(), pipelineEnd = _pipelines.end(); pipelineIt != pipelineEnd; ++pipelineIt) {
        if (pipelineIt != mainPipeline)
            (**pipelineIt).thread = std::thread([this, &pipeline = **pipelineIt] { runPipeline(pipeline); });
    }

    // Run first pipeline in the main thread
    runPipeline(**mainPipeline, std::move(mainThreadInlineTick));
}

void ECS::Executor::runPipeline(Pipeline &pipeline, Core::TrivialFunctor<void(void)> &&inlineTick) noexcept
{
    // Minimum time to sleep
    constexpr std::int64_t MinimumSleepDuration = 1'000'000;

    // Build the system graph of the pipeline
    buildPipelineGraph(pipeline);

    // While executor is running, continue to run the pipeline at its frequency
    auto now = std::chrono::high_resolution_clock::now();
    auto nextTick = now;
    while (_running.load(std::memory_order_relaxed)) {
        now = std::chrono::high_resolution_clock::now();
        const auto remaining = std::chrono::duration_cast<std::chrono::nanoseconds>(nextTick - now).count();
        if (remaining > MinimumSleepDuration) {
            // kFInfo(pipeline.name, "\tSleeping: ", remaining);
            Flow::PreciseSleep(remaining - MinimumSleepDuration);
        // Tick now
        } else {
            if (pipeline.graph.running()) {
                kFInfo(pipeline.name, "\tWaiting");
                pipeline.graph.wait();
            }
            if (inlineTick)
                inlineTick();
            // kFInfo(pipeline.name, "\tSchedule");
            _scheduler.schedule(pipeline.graph);
            const auto x = std::chrono::duration_cast<std::chrono::nanoseconds>(nextTick - std::chrono::high_resolution_clock::now()).count();
            if (x < 0)
                kFInfo(pipeline.name, " late by ", x, "ns");
            nextTick = now + std::chrono::nanoseconds(pipeline.tickRate);
        }
    }
}

// void ECS::Executor::observePipelines(void) noexcept
// {
//     const std::int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
//     const std::int64_t elapsed = now - _cache.lastTick;
//     std::int64_t next = INT64_MAX;

//     // kFInfo("[ECS Executor] Observe pipelines ", elapsed);
//     // Iterate over each pipeline clock
//     for (PipelineIndex pipelineIndex = 0; auto &clock : _pipelines.clocks) {
//         const auto tickRate = clock.tickRate(); // @todo fix this datarace when clock frequency changes at runtime (mutex / atomic)
//         const bool isTimeBound = clock.isTimeBound();
//         clock.elapsed += elapsed;
//         // If the pipeline has to be executed
//         if (clock.elapsed >= tickRate) [[unlikely]] {
//             // If the graph is not being executed, schedule it
//             if (auto &graph = *_pipelines.graphs.at(pipelineIndex); !graph.running()) [[likely]] {
//                 // kFInfo("[ECS Executor] | Tick '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
//                 if (isTimeBound)
//                     clock.elapsed -= tickRate;
//                 else
//                     clock.elapsed = 0;
//                 if (const auto &inlineBeginPass = _pipelines.inlineBeginPasses.at(pipelineIndex); !inlineBeginPass || inlineBeginPass()) [[likely]]
//                     _scheduler.schedule(graph);
//                 next = std::min(next, now + tickRate);
//             // Else we must schedule the graph as soon as it finishes execution
//             } else {
//                 // kFInfo("[ECS Executor] | Busy '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
//                 next = now;
//             }
//         // The pipeline have time before schedule
//         } else {
//             // kFInfo("[ECS Executor] | Wait '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
//             next = std::min(next, now + tickRate - clock.elapsed);
//         }
//         ++pipelineIndex;
//     }
//     _cache.lastTick = now;
//     _cache.nextTick = next;
// }

// void ECS::Executor::waitPipelines(void) noexcept
// {
//     while (true) {
//         const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
//         const std::int64_t nanoseconds = _cache.nextTick - now;
//         // Don't sleep, we are late
//         if (nanoseconds <= 0)
//             break;
//         // Sleep for the whole duration if we have enough time to sleep
//         else if (nanoseconds > 500'000) {
//             Flow::PreciseSleep(nanoseconds);
//             // const auto end = std::chrono::high_resolution_clock::now().time_since_epoch().count();
//             // const auto observed = end - now;
//             // if (end > _cache.nextTick + 100'000)
//             //     kFInfo("[ECS Executor] Oversleep ", observed / 1e6, " / ", nanoseconds / 1e6, "ms ", (double(observed) / double(nanoseconds)) * 100.0, "%");
//         // Spin wait
//         } else
//             std::this_thread::yield();
//     }
// }

void ECS::Executor::buildPipelineGraph(Pipeline &pipeline) noexcept
{
    // Clear old graph
    pipeline.graph.clear();

    // The first taks of the pipeline executes events and tells if the pipeline can execute
    auto &beginTask = pipeline.graph.add([&pipeline] {
        // Process all pipeline events
        PipelineEvent event;
        while (pipeline.events->pop(event))
            event();

        // If we have a begin pass function, call it to know if the pipeline must execute
        if (pipeline.beginPass)
            return pipeline.beginPass();
        else
            return false;
    });

    // If pipeline does not contain any system, don't build further
    if (pipeline.systems.empty()) [[unlikely]]
        return;

    Flow::Task *prevTickTask = &beginTask;
    Flow::Task *prevGraphTask = nullptr;

    // For each system, record tick & graph tasks then link them to begin / end
    for (auto &system : pipeline.systems) {
        auto &tickTask = pipeline.graph.add([system = system.get()](void) -> bool { return !system->tick(); });
        auto &graphTask = pipeline.graph.add(&system->taskGraph());

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