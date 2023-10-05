/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Random number generator
 */


#include <random>
#include <thread>

#include <pcg_random.hpp>

#include "Random.hpp"

static thread_local pcg32_fast LocalGenerator32 {
    static_cast<std::uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
};

static thread_local pcg64_fast LocalGenerator64 {
    std::hash<std::thread::id>{}(std::this_thread::get_id())
};

std::uint32_t kF::Core::Random::Generate32(void) noexcept
{
    return LocalGenerator32();
}

std::uint32_t kF::Core::Random::Generate32(const std::uint32_t to) noexcept
{
    return LocalGenerator32(to);
}

std::uint64_t kF::Core::Random::Generate64(void) noexcept
{
    return LocalGenerator64();
}

std::uint64_t kF::Core::Random::Generate64(const std::uint64_t to) noexcept
{
    return LocalGenerator64(to);
}
