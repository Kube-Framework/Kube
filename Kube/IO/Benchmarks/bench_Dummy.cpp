/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of IO
 */

#include <benchmark/benchmark.h>

static void IO_Dummy(benchmark::State &state)
{
    for (auto _ : state) {
    }
}
BENCHMARK(IO_Dummy);
