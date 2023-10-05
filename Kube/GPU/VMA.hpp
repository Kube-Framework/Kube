#pragma once

#include <Kube/Core/Platform.hpp>

#define VMA_VULKAN_VERSION 1002000

#if KUBE_COMPILER_GCC | KUBE_COMPILER_CLANG
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wunused-parameter"
# pragma GCC diagnostic ignored "-Wunused-variable"
# pragma GCC diagnostic ignored "-Wparentheses"
# pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
# pragma GCC diagnostic ignored "-Wconversion"
# pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <cstdio> // Required in GCC 13
#include <vk_mem_alloc.h>

#if KUBE_COMPILER_GCC | KUBE_COMPILER_CLANG
# pragma GCC diagnostic pop
#endif