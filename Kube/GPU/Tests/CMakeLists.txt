kube_add_unit_tests(GPUTests
    SOURCES
        GPUTestUtils.hpp
        tests_MemoryAllocation.cpp
        tests_GPU.cpp

    LIBRARIES
        GPU
)
