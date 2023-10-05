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

static void PreciseSleep(const std::int64_t nanoseconds) noexcept;

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
    : _scheduler(workerCount, taskQueueSize), _eventQueue(eventQueueSize, false)
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
    _pipelines.clocks.at(pipelineIndex).setTickRate(HzToRate(frequencyHz));
}

void ECS::Executor::stop(void) noexcept
{
    const auto res = _eventQueue.push([this] {
        waitIDLE();
        return false;
    });
    kFEnsure(res, "ECS::Executor::stop: Critical event couldn't not get received");
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
        if (event()) [[likely]]
            continue;
        return false;
    }
    return true;
}

void ECS::Executor::observePipelines(void) noexcept
{
    const std::int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const std::int64_t elapsed = now - _cache.lastTick;
    std::int64_t next = INT64_MAX;

    // kFInfo("Observe pipelines ", elapsed);
    // Iterate over each pipeline clock
    for (PipelineIndex pipelineIndex = 0; auto &clock : _pipelines.clocks) {
        const auto tickRate = clock.tickRate(); // @todo fix this datarace when clock frequency changes at runtime (mutex / atomic)
        const bool isTimeBound = clock.isTimeBound();
        clock.elapsed += elapsed;
        // If the pipeline has to be executed
        if (clock.elapsed >= tickRate) [[unlikely]] {
            // If the graph is not being executed, schedule it
            if (auto &graph = *_pipelines.graphs.at(pipelineIndex); !graph.running()) [[likely]] {
                // kFInfo("  Tick '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
                if (isTimeBound)
                    clock.elapsed -= tickRate;
                else
                    clock.elapsed = 0;
                if (const auto &inlineBeginPass = _pipelines.inlineBeginPasses.at(pipelineIndex); !inlineBeginPass || inlineBeginPass()) [[likely]]
                    _scheduler.schedule(graph);
                next = std::min(next, now + tickRate);
            // Else we must schedule the graph as soon as it finishes execution
            } else {
                // kFInfo("  Busy '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
                next = now;
            }
        // The pipeline have time before schedule
        } else {
            // kFInfo("  Wait '", _pipelines.names.at(pipelineIndex), "': ", clock.elapsed / static_cast<double>(tickRate), " (", clock.elapsed, " / ", tickRate, ")");
            next = std::min(next, now + tickRate - clock.elapsed);
        }
        ++pipelineIndex;
    }
    _cache.lastTick = now;
    _cache.nextTick = next;
}

void ECS::Executor::waitPipelines(void) noexcept
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto seconds = static_cast<double>(_cache.nextTick - now) / 1e9;

    // If we are late, no time to loose
    if (seconds <= 0.0)
        return;

    // Sleep wait
    // kFInfo("Sleeping for ", seconds * 1e3, " ms (", _cache.sleepEstimate * 1e3, ")");
    while (seconds > _cache.sleepEstimate) {
        // Sleep for 1ms
        auto start = std::chrono::high_resolution_clock::now();
        PreciseSleep(1'000'000);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = std::chrono::high_resolution_clock::now();

        // Observe real slept time
        const auto observed = static_cast<double>((end - start).count()) / 1e9;
        // kFInfo("  Slept for ", observed * 1e3, " / ", seconds * 1e3, " ms (", _cache.sleepEstimate * 1e3, ")");
        seconds -= observed;

        // Update estimate accurate sleep time
        ++_cache.sleepCount;
        double delta = observed - _cache.sleepMean;
        _cache.sleepMean += delta / static_cast<double>(_cache.sleepCount);
        _cache.sleepM2   += delta * (observed - _cache.sleepMean);
        double stddev = std::sqrt(_cache.sleepM2 / static_cast<double>(_cache.sleepCount - 1));
        _cache.sleepEstimate = _cache.sleepMean + stddev;
    }

    // Spin wait
    while (std::chrono::high_resolution_clock::now().time_since_epoch().count() < _cache.nextTick)
        std::this_thread::yield();
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

#if KUBE_PLATFORM_WINDOWS
#include <Windows.h>
struct WindowsTimer
{
    HANDLE handle;

    WindowsTimer(void) noexcept
    {
        handle = CreateWaitableTimer(NULL, FALSE, NULL);
        kFEnsure(handle,
            "ECS::Executor::PreciseSleep: Couldn't create windows timer handle");
    }

    ~WindowsTimer(void) noexcept { CloseHandle(handle); }
};

static void PreciseSleep(const std::int64_t nanoseconds) noexcept
{
    static WindowsTimer WindowsTimer {};

    // Setup timer & wait
    LARGE_INTEGER timeDef;
    timeDef.QuadPart = -nanoseconds / 100;
    if (SetWaitableTimer(WindowsTimer.handle, &timeDef, 0, nullptr, nullptr, false)) [[likely]]
        WaitForSingleObject(WindowsTimer.handle, INFINITE);
}
#else
#include <time.h>
static void PreciseSleep(const std::int64_t nanoseconds) noexcept
{
    const struct timespec spec {
        .tv_sec = time_t {},
        .tv_nsec = nanoseconds
    };
    struct timespec rem {};
    nanosleep(&spec, &rem);
}
#endif

void ECS::Executor::waitIDLE(void) noexcept
{
    for (auto &graph : _pipelines.graphs)
        graph->waitSpin();
}
