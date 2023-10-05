/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of Audio
 */

#include <benchmark/benchmark.h>

static void Audio_Dummy(benchmark::State &state)
{
    for (auto _ : state) {
    }
}
BENCHMARK(Audio_Dummy);
