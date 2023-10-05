/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Random tests
 */

#include <thread>
#include <vector>
#include <array>

#include <gtest/gtest.h>

#include <Kube/Core/Random.hpp>

using namespace kF;

template<typename Type>
void TestStatistics(void)
{
    constexpr std::size_t Count = 1'000'000u;
    constexpr std::size_t FailureMinThreshold = Count / 100u / 2u; // Allows -50% error
    constexpr std::size_t FailureMaxThreshold = Count / 100u + FailureMinThreshold; // +50%

    // Generate Count values
    std::vector<Type> vec(Count);
    for (auto &num : vec) {
        if constexpr (std::is_same_v<Type, std::uint32_t>)
            num = Core::Random::Generate32(100u);
        else
            num = Core::Random::Generate64(100u);
    }

    // Collect statistics
    std::array<Type, 100> counters {};
    for (auto &num : vec) {
        ASSERT_GE(num, 0u);
        ASSERT_LT(num, 100u);
        ++counters[num];
    }
    // Analyse statistics
    for (const auto counter : counters) {
        ASSERT_GE(counter, FailureMinThreshold);
        ASSERT_LE(counter, FailureMaxThreshold);
    }
}

TEST(Random32, Statistics)
{
    std::thread thds[8];

    for (auto &thd : thds) {
        thd = std::thread([] {
            TestStatistics<std::uint32_t>();
        });
    }

    for (auto &thd : thds) {
        thd.join();
    }
}

TEST(Random64, Statistics)
{
    std::thread thds[8];

    for (auto &thd : thds) {
        thd = std::thread([] {
            TestStatistics<std::uint64_t>();
        });
    }

    for (auto &thd : thds) {
        thd.join();
    }
}