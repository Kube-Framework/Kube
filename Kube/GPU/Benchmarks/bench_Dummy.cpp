/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of GPU
 */

#include <benchmark/benchmark.h>

static void GPU_Dummy(benchmark::State &state)
{
    for (auto _ : state) {
    }
}
BENCHMARK(GPU_Dummy);
