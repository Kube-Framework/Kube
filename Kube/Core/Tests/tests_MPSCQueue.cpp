/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the MPSC Queue
 */

#include <thread>

#include <gtest/gtest.h>

#include <Kube/Core/Debug.hpp>
#include <Kube/Core/MPSCQueue.hpp>

using namespace kF;

constexpr auto LongStr = "123456789123456789";
constexpr auto ShortStr = "12345";

TEST(MPSCQueue, SinglePushPop)
{
    constexpr std::size_t queueSize = 8;

    Core::MPSCQueue<std::string> queue(queueSize);

    for (auto i = 0u; i < queueSize; ++i)
        ASSERT_TRUE(queue.push(LongStr));
    for (auto i = 0u; i < queueSize; ++i)
        ASSERT_FALSE(queue.push(ShortStr));
    for (auto i = 0u; i < queueSize; ++i) {
        std::string str;
        ASSERT_TRUE(queue.pop(str));
        ASSERT_EQ(str, LongStr);
    }
}

TEST(MPSCQueue, RangePushPop)
{
    constexpr auto test = [](auto &queue, const std::size_t size) {
        const char ref = static_cast<char>(size % static_cast<std::size_t>(INT8_MAX));
        std::vector<char> tmp(size);
        for (auto &c : tmp)
            c = ref;
        ASSERT_TRUE(queue.tryPushRange(tmp.begin(), tmp.end()));
        for (auto &c : tmp)
            c = 0;
        ASSERT_TRUE(queue.tryPopRange(tmp.begin(), tmp.end()));
        for (const auto c : tmp)
            ASSERT_EQ(c, ref);
    };

    constexpr std::size_t maxQueueSize = KUBE_DEBUG_BUILD ? 64 : 4096;

    for (auto queueSize = 1ul; queueSize < maxQueueSize; queueSize *= 2) {
        std::vector<char> tmp(queueSize);
        Core::MPSCQueue<char> queue(queueSize);
        ASSERT_FALSE(queue.tryPushRange(tmp.data(), tmp.data() + queueSize + 1));
        ASSERT_FALSE(queue.tryPopRange(tmp.data(), tmp.data() + queueSize + 1));
        for (auto size = 1ul; size <= queueSize; ++size)
            test(queue, size);
        for (auto size = queueSize; size > 0; --size)
            test(queue, size);
    }

    Core::MPSCQueue<char> queue(maxQueueSize);
    std::vector<char> tmp(maxQueueSize * 2);
    for (auto &c : tmp)
        c = 42;
    ASSERT_EQ(queue.pushRange(tmp.begin(), tmp.end()), maxQueueSize);
    ASSERT_EQ(queue.popRange(tmp.begin(), tmp.end()), maxQueueSize);
}

TEST(MPSCQueue, IntensiveThreading)
{
    constexpr auto ThreadCount = KUBE_DEBUG_BUILD ? 2 : 4;
    constexpr auto Counter = KUBE_DEBUG_BUILD ? 64 : 4096;
    constexpr std::size_t queueSize = KUBE_DEBUG_BUILD ? 64 : 4096;

    static std::atomic<bool> running { true };
    static std::atomic<std::size_t> pushingThds { 0 };
    static std::atomic<std::size_t> popCount { 0 };

    std::thread pushThds[ThreadCount];
    std::thread popThd;

    Core::MPSCQueue<int> queue(queueSize);

    for (auto i = 0; i < ThreadCount; ++i) {
        pushThds[i] = std::thread([&queue] {
            ++pushingThds;
            for (auto i = 0; i < Counter / ThreadCount;)
                i += queue.push(i);
            --pushingThds;
        });
    }
    popThd = std::thread([&queue] {
        while (running) {
            int tmp = 0;
            while (queue.pop(tmp))
                ++popCount;
        }
    });

    while (pushingThds)
        std::this_thread::yield();
    for (auto i = 0; i < ThreadCount; ++i) {
        if (pushThds[i].joinable())
            pushThds[i].join();
    }
    while (popCount != Counter)
        std::this_thread::yield();
    running = false;
    if (popThd.joinable())
        popThd.join();
}

TEST(MPSCQueue, IntensiveThreadingRange)
{
    constexpr auto ThreadCount = KUBE_DEBUG_BUILD ? 2 : 4;
    constexpr auto Range = KUBE_DEBUG_BUILD ? 8 : 64;
    constexpr auto Counter = KUBE_DEBUG_BUILD ? 64 : 4096;
    constexpr std::size_t queueSize = KUBE_DEBUG_BUILD ? 64 : 4096;

    static_assert(Counter == (Counter / Range) * Range);

    static std::atomic<bool> running { true };
    static std::atomic<std::size_t> pushingThds { 0 };
    static std::atomic<std::size_t> popCount { 0 };

    std::thread pushThds[ThreadCount];
    std::thread popThd;

    Core::MPSCQueue<int> queue(queueSize);

    for (auto i = 0; i < ThreadCount; ++i) {
        pushThds[i] = std::thread([&queue] {
            ++pushingThds;
            int cache[Range];
            for (int i = 0; i < Range; ++i)
                cache[i] = i;
            for (std::size_t i = 0; i < Counter / ThreadCount;)
                i += queue.pushRange(std::begin(cache), std::end(cache));
            --pushingThds;
        });
    }
    popThd = std::thread([&queue] {
        int cache[Range];
        while (running) {
            popCount += queue.popRange(std::begin(cache), std::end(cache));
        }
    });

    while (pushingThds)
        std::this_thread::yield();
    for (auto i = 0; i < ThreadCount; ++i) {
        if (pushThds[i].joinable())
            pushThds[i].join();
    }
    while (popCount != Counter)
        std::this_thread::yield();
    running = false;
    if (popThd.joinable())
        popThd.join();
}