/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of Flow
 */

#include <benchmark/benchmark.h>

static void Flow_Dummy(benchmark::State &state)
{
    for (auto _ : state) {
    }
}
BENCHMARK(Flow_Dummy);
