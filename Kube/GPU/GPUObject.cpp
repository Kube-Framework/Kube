/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPUObject
 */

#include "GPU.hpp"

using namespace kF;

struct alignas(alignof(GPU::GPU)) GPUCache
{
    std::byte bytes[sizeof(GPU::GPU)];
};

static GPUCache ParentGPUBytes {};

GPU::GPU &GPU::GPUObject::Parent(void) noexcept
{
    return reinterpret_cast<GPU &>(ParentGPUBytes);
}