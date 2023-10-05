/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of allocator classes
 */

#include <cstdlib>
#include <memory_resource>
#include <vector>
#include <chrono>
#include <thread>

#include <benchmark/benchmark.h>

#include <Kube/Core/MacroUtils.hpp>
#include <Kube/Core/UnsafeAllocator.hpp>
#include <Kube/Core/AllocatedVector.hpp>

using namespace kF;

struct Allocation
{
    void *data {};
    std::size_t size {};
    std::size_t alignment {};
};

struct NoAllocator
{
    [[nodiscard]] void *allocate(const std::size_t size, const std::size_t alignment) noexcept
        { return Core::AlignedAlloc(size, alignment); }

    void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept
        { return Core::AlignedFree(data, size, alignment); }
};

#include <iostream>

class DebugAllocator : public std::pmr::memory_resource
{
public:
    ~DebugAllocator(void) override = default;

    [[nodiscard]] void *do_allocate(std::size_t size, std::size_t alignment) override
    {
        auto ptr = Core::AlignedAlloc(size, alignment);
        std::cout << "[Allocate] " << ptr << " | " << size << " | " << alignment << std::endl;
        return ptr;
    }

    void do_deallocate(void *data, std::size_t size, std::size_t alignment) override
    {
        std::cout << "[Deallocate] " << data << " | " << size << " | " << alignment << std::endl;
        return Core::AlignedFree(data, size, alignment);
    }

    [[nodiscard]] bool do_is_equal(const memory_resource &other) const noexcept override { return this == &other; }
};

using RetentionVector = std::vector<Allocation>;


template<typename Allocator, std::size_t Size, std::size_t Alignment>
void AllocDealloc_FixedSize(benchmark::State &state)
{
    Allocator allocator;

    while (state.KeepRunning()) {
        auto start = std::chrono::high_resolution_clock::now();

        auto ptr = allocator.allocate(Size, Alignment);
        benchmark::DoNotOptimize(ptr);
        allocator.deallocate(ptr, Size, Alignment);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed.count());
    }
}

template<typename Allocator, std::size_t Size, std::size_t Alignment>
void Alloc_FixedSize(benchmark::State &state)
{
    Allocator allocator;

    while (state.KeepRunning()) {
        auto start = std::chrono::high_resolution_clock::now();

        auto ptr = allocator.allocate(Size, Alignment);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed.count());

        allocator.deallocate(ptr, Size, Alignment);
    }
}

template<typename Allocator, std::size_t Size, std::size_t Alignment>
void Dealloc_FixedSize(benchmark::State &state)
{
    Allocator allocator;

    while (state.KeepRunning()) {
        auto ptr = allocator.allocate(Size, Alignment);
        auto start = std::chrono::high_resolution_clock::now();

        allocator.deallocate(ptr, Size, Alignment);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed.count());
    }
}

template<typename Allocator, std::size_t Size, std::size_t Alignment, std::size_t RetentionCount, std::size_t RetentionCleanupCount>
void Retention_FixedSize(benchmark::State &state)
{
    static_assert(RetentionCount > 0);
    static_assert(RetentionCleanupCount <= RetentionCount);

    // std::cout << "\n--- Retention_FixedSize ---" << std::endl;
    // DebugAllocator dbg;
    // Allocator allocator(&dbg);
    Allocator allocator;
    RetentionVector retentions;
    retentions.reserve(RetentionCount);

    while (state.KeepRunning()) {
        auto start = std::chrono::high_resolution_clock::now();

        auto ptr = allocator.allocate(Size, Alignment);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed.count());

        retentions.push_back(Allocation {
            ptr,
            Size,
            Alignment
        });
        if (retentions.size() == RetentionCount) {
            const auto begin = retentions.end() - RetentionCleanupCount;
            const auto end = retentions.end();
            for (auto it = begin; it != end; ++it)
                allocator.deallocate(it->data, it->size, it->alignment);
            retentions.erase(begin, end);
        }
    }
}

template<typename Allocator, typename Type, std::size_t PushCount>
void VectorPushBack_FixedSize(benchmark::State &state)
{
    Allocator allocator;

    while (state.KeepRunning()) {
        Core::AllocatedVector<Type> vector(allocator);
        auto start = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0u; i < PushCount; ++i) {
            // auto data = vector.data();
            vector.push(static_cast<Type>(i));
            // if (data != vector.data())
                // std::cout << "Vector reallocated " << vector.data() << " | " << vector.size() << " | " << vector.capacity() << std::endl;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    }
}

template<typename Allocator, typename Type, std::size_t PushCount>
void NoisyThread_VectorPushBack_FixedSize(benchmark::State &state)
{
    std::atomic_bool running { true };
    std::thread thd([&running] {
        while (running) {
            std::vector<void *> allocs;
            for (std::size_t i = 8; i < (1u << 12u); i <<= 1) {
                allocs.push_back(
                    Core::AlignedAlloc(i, std::max(i, static_cast<std::size_t>(4096u)))
                );
            }
            auto it = allocs.begin();
            for (std::size_t i = 8; i < (1u << 12u); i <<= 1) {
                Core::AlignedFree(*it, i, std::max(i, static_cast<std::size_t>(4096u)));
                ++it;
            }
        }
    });

    // std::cout << "\n--- VectorPushBack_FixedSize ---" << std::endl;
    // DebugAllocator dbg;
    // Allocator allocator(&dbg);
    Allocator allocator;


    while (state.KeepRunning()) {
        Core::AllocatedVector<Type> vector(allocator);
        auto start = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0u; i < PushCount; ++i) {
            // auto data = vector.data();
            vector.push(static_cast<Type>(i));
            // if (data != vector.data())
                // std::cout << "Vector reallocated " << vector.data() << " | " << vector.size() << " | " << vector.capacity() << std::endl;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    }

    running = false;
    if (thd.joinable())
        thd.join();
}

#define GENERATE_ALLOCDEALLOC_FIXEDSIZE(AllocatorName, AllocatorType, Size, Alignment) \
static void AllocatorName##_AllocDealloc_FixedSize_##Size##_##Alignment(benchmark::State &state) \
    { return AllocDealloc_FixedSize<AllocatorType, Size, Alignment>(state); } \
BENCHMARK(AllocatorName##_AllocDealloc_FixedSize_##Size##_##Alignment)

#define GENERATE_ALLOC_FIXEDSIZE(AllocatorName, AllocatorType, Size, Alignment) \
static void AllocatorName##_Alloc_FixedSize_##Size##_##Alignment(benchmark::State &state) \
    { return Alloc_FixedSize<AllocatorType, Size, Alignment>(state); } \
BENCHMARK(AllocatorName##_Alloc_FixedSize_##Size##_##Alignment)

#define GENERATE_DEALLOC_FIXEDSIZE(AllocatorName, AllocatorType, Size, Alignment) \
static void AllocatorName##_Dealloc_FixedSize_##Size##_##Alignment(benchmark::State &state) \
    { return Dealloc_FixedSize<AllocatorType, Size, Alignment>(state); } \
BENCHMARK(AllocatorName##_Dealloc_FixedSize_##Size##_##Alignment)

#define GENERATE_RETENTION_FIXEDSIZE(AllocatorName, AllocatorType, Size, Alignment, RetentionCount, RetentionCleanupCount) \
static void AllocatorName##_Retention_FixedSize_##Size##_##Alignment##_##RetentionCount##_##RetentionCleanupCount(benchmark::State &state) \
    { return Retention_FixedSize<AllocatorType, Size, Alignment, RetentionCount, RetentionCleanupCount>(state); } \
BENCHMARK(AllocatorName##_Retention_FixedSize_##Size##_##Alignment##_##RetentionCount##_##RetentionCleanupCount)

#define GENERATE_VECTORPUSHBACK_FIXEDSIZE(AllocatorName, AllocatorType, PushCount, TypeName, Type) \
static void AllocatorName##_VectorPushBack_FixedSize_##TypeName##_##PushCount(benchmark::State &state) \
    { return VectorPushBack_FixedSize<AllocatorType, Type, PushCount>(state); } \
BENCHMARK(AllocatorName##_VectorPushBack_FixedSize_##TypeName##_##PushCount)


#define GENERATE_NOISYTHREAD_VECTORPUSHBACK_FIXEDSIZE(AllocatorName, AllocatorType, PushCount, TypeName, Type) \
static void AllocatorName##_NoisyThread_VectorPushBack_FixedSize_##TypeName##_##PushCount(benchmark::State &state) \
    { return NoisyThread_VectorPushBack_FixedSize<AllocatorType, Type, PushCount>(state); } \
BENCHMARK(AllocatorName##_NoisyThread_VectorPushBack_FixedSize_##TypeName##_##PushCount)


#define GENERATE_POW2_TESTS(AllocatorName, AllocatorType, TESTNAME, ...) \
    TESTNAME(AllocatorName, AllocatorType, 8 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 16 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 32 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 64 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 256 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 512 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 1024 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 2048 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 4096 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 8192 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 16384 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 32768 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 65536 __VA_OPT__(, __VA_ARGS__));

#define GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, TESTNAME, ...) \
    TESTNAME(AllocatorName, AllocatorType, 8, 8 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 16, 16 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 32, 32 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 64, 64 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 128, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 256, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 512, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 1024, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 2048, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 4096, 128 __VA_OPT__(, __VA_ARGS__)); \
    TESTNAME(AllocatorName, AllocatorType, 8192, 128 __VA_OPT__(, __VA_ARGS__));


#define GENERATE_TESTS_INSTANCE(AllocatorName, AllocatorType) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_ALLOCDEALLOC_FIXEDSIZE) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_ALLOC_FIXEDSIZE) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_DEALLOC_FIXEDSIZE) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 100, 50) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 100, 100) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 1000, 500) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 1000, 1000) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 10000, 5000) \
    GENERATE_POW2_ALIGNED_TESTS(AllocatorName, AllocatorType, GENERATE_RETENTION_FIXEDSIZE, 10000, 10000) \
    GENERATE_POW2_TESTS(AllocatorName, AllocatorType, GENERATE_VECTORPUSHBACK_FIXEDSIZE, uint32, std::uint32_t) \
    GENERATE_POW2_TESTS(AllocatorName, AllocatorType, GENERATE_VECTORPUSHBACK_FIXEDSIZE, uint64, std::uint64_t) \
    GENERATE_POW2_TESTS(AllocatorName, AllocatorType, GENERATE_NOISYTHREAD_VECTORPUSHBACK_FIXEDSIZE, uint32, std::uint32_t) \
    GENERATE_POW2_TESTS(AllocatorName, AllocatorType, GENERATE_NOISYTHREAD_VECTORPUSHBACK_FIXEDSIZE, uint64, std::uint64_t) \

    // GENERATE_POW2_TESTS(AllocatorName, AllocatorType, GENERATE_VECTORPUSHBACK_FIXEDSIZE, string, std::string)

// GENERATE_TESTS_INSTANCE(NoAllocator, NoAllocator)
// GENERATE_TESTS_INSTANCE(UnsynchronizedStandardAllocator, std::pmr::unsynchronized_pool_resource)
GENERATE_TESTS_INSTANCE(UnsafeAllocator, Core::UnsafeAllocator<>)
