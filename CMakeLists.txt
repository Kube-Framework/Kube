# Set cmake minimum requirements
cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# Include toolchain options
include(CompileOptions/ToolchainOptions.cmake)


# Declare Kube project after toolchain options
project(Kube VERSION 0.1.0)


# Enable Werror
option(KF_WERROR "Enable -Werror flag with GCC / Clang" OFF)

# Enable full debuging
option(KF_FULL_DEBUG "Enable -fno-omit-frame-pointer -fsanitize=address flags with GCC / Clang" ON)

# Enable examples
option(KF_EXAMPLES "Build examples" OFF)

# Enable unit testing
option(KF_TESTS "Build tests" OFF)

# Enable coverage over unit tests
option(KF_COVERAGE "Enable coverage" OFF)

# Enable benchmarking
option(KF_BENCHMARKS "Build benchmarks" OFF)


# Include compile options
include(CompileOptions/CompileOptions.cmake)


# Target helpers
include(CompileOptions/Utils.cmake)

# Library helper
function(kube_add_library LibraryName)
    utils_add_library(${LibraryName} TESTS ${KF_TESTS} BENCHMARKS ${KF_BENCHMARKS} ${ARGN} INCLUDES ${PROJECT_SOURCE_DIR})
    add_library(Kube::${LibraryName} ALIAS ${LibraryName})
endfunction()

# Executable helper
function(kube_add_executable ExecutableName)
    utils_add_executable(${ExecutableName} TESTS ${KF_TESTS} BENCHMARKS ${KF_BENCHMARKS} ${ARGN} INCLUDES ${PROJECT_SOURCE_DIR})
endfunction()

# Unit tests helper
function(kube_add_unit_tests ExecutableName)
    utils_add_unit_tests(${ExecutableName} COVERAGE ${KF_COVERAGE} ${ARGN} INCLUDES ${PROJECT_SOURCE_DIR})
endfunction()

# Benchmarks helper
function(kube_add_benchmarks ExecutableName)
    utils_add_benchmarks(${ExecutableName} ${ARGN} INCLUDES ${PROJECT_SOURCE_DIR})
endfunction()


# Include external libraries
add_subdirectory(External)

# Include Kube framework
add_subdirectory(Kube)

# Build examples
if(${KF_EXAMPLES})
    add_subdirectory(Examples)
endif()