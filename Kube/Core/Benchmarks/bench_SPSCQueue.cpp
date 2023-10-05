/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of SPSCQueue class
 */

#include <thread>

#include <benchmark/benchmark.h>

#include <Kube/Core/SPSCQueue.hpp>

using namespace kF;

using Queue = Core::SPSCQueue<std::size_t>;

#define GENERATE_TESTS(TEST, ...) \
    TEST(4096 __VA_OPT__(,) __VA_ARGS__)

#define GENERATE_RANGE_TESTS(TEST, ...) \
    TEST(4096, 4 __VA_OPT__(,) __VA_ARGS__)

#define SPSCQUEUE_SINGLETHREADED_PUSH(Capacity) \
static void SPSCQueue_SingleThreadedPush_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    for (auto _ : state) { \
        queue.clear(); \
        auto start = std::chrono::high_resolution_clock::now(); \
        while (queue.push(42ul)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
} \
BENCHMARK(SPSCQueue_SingleThreadedPush_##Capacity)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_SINGLETHREADED_PUSH);

#define SPSCQUEUE_SINGLETHREADED_POP(Capacity) \
static void SPSCQueue_SingleThreadedPop_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    for (auto _ : state) { \
        while (queue.push(42ul)); \
        std::size_t tmp; \
        auto start = std::chrono::high_resolution_clock::now(); \
        while (queue.pop(tmp)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
} \
BENCHMARK(SPSCQueue_SingleThreadedPop_##Capacity)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_SINGLETHREADED_POP);

#define SPSCQUEUE_NOISY_PUSH(Capacity) \
static void SPSCQueue_NoisyPush_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::atomic<bool> running = true; \
    std::thread thd([&queue, &running] { for (std::size_t tmp; running; benchmark::DoNotOptimize(queue.pop(tmp))); }); \
    for (auto _ : state) { \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.push(42ul)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyPush_##Capacity)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_NOISY_PUSH);

#define SPSCQUEUE_NOISY_POP(Capacity) \
static void SPSCQueue_NoisyPop_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::atomic<bool> running = true; \
    std::thread thd([&queue, &running] { while (running) benchmark::DoNotOptimize(queue.push(42ul)); }); \
    for (auto _ : state) { \
        std::size_t tmp; \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.pop(tmp)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyPop_##Capacity)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_NOISY_POP);

#define SPSCQUEUE_NOISY_RANGEPUSH(Capacity, Div) \
static void SPSCQueue_NoisyRangePush_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::atomic<bool> running = true; \
    std::thread thd([&queue, &running] { \
        for (std::size_t cache[Capacity / Div]; running; benchmark::DoNotOptimize(queue.tryPopRange(cache, cache + Capacity / Div))); \
    }); \
    std::size_t cache[Capacity / Div]; \
    for (auto &c : cache) \
        c = 42ul; \
    for (auto _ : state) { \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.tryPushRange(cache, cache + Capacity / Div)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyRangePush_##Capacity)->UseManualTime();

GENERATE_RANGE_TESTS(SPSCQUEUE_NOISY_RANGEPUSH);

#define SPSCQUEUE_NOISY_RANGEPOP(Capacity, Div) \
static void SPSCQueue_NoisyRangePop_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::atomic<bool> running = true; \
    std::thread thd([&queue, &running] { \
        std::size_t cache[Capacity / Div]; \
        for (auto &c : cache) \
            c = 42ul; \
        for (; running; benchmark::DoNotOptimize(queue.tryPushRange(cache, cache + Capacity / Div))); \
    }); \
    std::size_t cache[Capacity / Div]; \
    for (auto _ : state) { \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.tryPopRange(cache, cache + Capacity / Div)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyRangePop_##Capacity)->UseManualTime();

GENERATE_RANGE_TESTS(SPSCQUEUE_NOISY_RANGEPOP);

#define SPSCQUEUE_RANGEPUSH(Capacity, Div) \
static void SPSCQueue_RangePush_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::size_t cache[Capacity / Div]; \
    for (auto &c : cache) \
        c = 42ul; \
    for (auto _ : state) { \
        auto start = std::chrono::high_resolution_clock::now(); \
        if (!queue.tryPushRange(cache, cache + Capacity / Div)) \
            return; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
        if (!queue.tryPopRange(cache, cache + Capacity / Div)) \
            return; \
    } \
} \
BENCHMARK(SPSCQueue_RangePush_##Capacity)->UseManualTime();

GENERATE_RANGE_TESTS(SPSCQUEUE_RANGEPUSH);

#define SPSCQUEUE_RANGEPOP(Capacity, Div) \
static void SPSCQueue_RangePop_##Capacity(benchmark::State &state) \
{ \
    Queue queue(Capacity); \
    std::size_t cache[Capacity / Div]; \
    for (auto &c : cache) \
        c = 42ul; \
    for (auto _ : state) { \
        if (!queue.tryPushRange(cache, cache + Capacity / Div)) \
            return; \
        auto start = std::chrono::high_resolution_clock::now(); \
        if (!queue.tryPopRange(cache, cache + Capacity / Div)) \
            return; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
} \
BENCHMARK(SPSCQueue_RangePop_##Capacity)->UseManualTime();

GENERATE_RANGE_TESTS(SPSCQUEUE_RANGEPOP);