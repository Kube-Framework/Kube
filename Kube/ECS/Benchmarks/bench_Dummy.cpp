/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of ECS
 */

#include <benchmark/benchmark.h>

static void ECS_Dummy(benchmark::State &state)
{
    for (auto _ : state) {
    }
}
BENCHMARK(ECS_Dummy);
