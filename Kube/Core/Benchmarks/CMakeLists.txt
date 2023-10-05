kube_add_benchmarks(CoreBenchmarks
    SOURCES
        bench_Allocator.cpp
        bench_SPSCQueue.cpp
        bench_MPMCQueue.cpp

    LIBRARIES
        Core
)