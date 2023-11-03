/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark helper
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    class Benchmark;
}

/** @brief Benchmark helper class */
class kF::Core::Benchmark
{
public:
    /** @brief Destructor, dump benchmark*/
    ~Benchmark(void) noexcept;

    /** @brief Constructor, acquire the begin timestamp */
    Benchmark(const std::string_view &name) noexcept;

    /** @brief Move constructor */
    inline Benchmark(Benchmark &&other) noexcept { swap(other); }

    /** @brief Move assignment */
    inline Benchmark &operator=(Benchmark &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    inline void swap(Benchmark &other) noexcept { std::swap(_name, other._name); std::swap(_beginTimestamp, other._beginTimestamp); std::swap(_lastStepTimestamp, other._lastStepTimestamp); }

    /** @brief Dump step benchmark */
    void step(const std::string_view &step) noexcept;

    /** @brief Release benchmark */
    void release(void) noexcept;

private:
    std::string_view _name {};
    std::int64_t _beginTimestamp {};
    std::int64_t _lastStepTimestamp {};
};