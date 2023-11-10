/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark helper
 */

#include <chrono>
#include <iomanip>

#include "Benchmark.hpp"

#include "Log.hpp"


using namespace kF;

static inline void PrintBenchmark(const std::int64_t beginTimestamp, const std::string_view &name, const std::string_view &step) noexcept
{
    const auto elapsedNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::time_point { std::chrono::high_resolution_clock::duration(beginTimestamp) }
    ).count();
    const auto elapsedMilliseconds = elapsedNanoseconds / 1'000'000;
    const auto remainingMicroseconds = (elapsedNanoseconds / 1'000) - (elapsedMilliseconds * 1'000);
    kFInfo('[', name, "] ", elapsedMilliseconds, '.', std::setfill('0'), std::setw(3), remainingMicroseconds, "ms\t", step);
}

Core::Benchmark::~Benchmark(void) noexcept
{
    if (!_name.empty())
        PrintBenchmark(_beginTimestamp, _name, "all");
}

Core::Benchmark::Benchmark(const std::string_view &name) noexcept
    : _name(name)
    , _beginTimestamp(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    , _lastStepTimestamp(_beginTimestamp)
{
}

void Core::Benchmark::step(const std::string_view &step) noexcept
{
    PrintBenchmark(_lastStepTimestamp, _name, step);
    _lastStepTimestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void Core::Benchmark::release(void) noexcept
{
    PrintBenchmark(_beginTimestamp, _name, "all");
    _name = {};
}