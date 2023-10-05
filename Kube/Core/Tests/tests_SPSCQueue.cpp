/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the SPSC Queue
 */

#include <thread>

#include <gtest/gtest.h>

#include <Kube/Core/Debug.hpp>
#include <Kube/Core/SPSCQueue.hpp>

using namespace kF;

constexpr auto LongStr = "123456789123456789";
constexpr auto ShortStr = "12345";

TEST(SPSCQueue, SinglePushPop)
{
    constexpr std::size_t queueSize = 8;

    Core::SPSCQueue<std::string> queue(queueSize);

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

TEST(SPSCQueue, RangePushPop)
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
        Core::SPSCQueue<char> queue(queueSize);
        ASSERT_FALSE(queue.tryPushRange(tmp.data(), tmp.data() + queueSize + 1));
        ASSERT_FALSE(queue.tryPopRange(tmp.data(), tmp.data() + queueSize + 1));
        for (auto size = 1ul; size <= queueSize; ++size)
            test(queue, size);
        for (auto size = queueSize; size > 0; --size)
            test(queue, size);
    }

    Core::SPSCQueue<char> queue(maxQueueSize);
    std::vector<char> tmp(maxQueueSize * 2);
    for (auto &c : tmp)
        c = 42;
    ASSERT_EQ(queue.pushRange(tmp.begin(), tmp.end()), maxQueueSize);
    ASSERT_EQ(queue.popRange(tmp.begin(), tmp.end()), maxQueueSize);
}

TEST(SPSCQueue, IntensiveThreading)
{
    constexpr auto Counter = KUBE_DEBUG_BUILD ? 64 : 4096;
    constexpr std::size_t queueSize = 4096;

    Core::SPSCQueue<int> queue(queueSize);

    std::thread thd([&queue] {
        for (auto i = 0; i < Counter; i += queue.push(i));
    });

    for (auto i = 0; i < Counter; ++i) {
        int tmp = 0;
        while (!queue.pop(tmp));
        ASSERT_EQ(tmp, i);
    }
    if (thd.joinable())
        thd.join();
}