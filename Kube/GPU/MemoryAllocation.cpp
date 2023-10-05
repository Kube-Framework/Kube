/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocation
 */

#include "GPU.hpp"
#include "MemoryAllocation.hpp"

using namespace kF;

GPU::MemoryAllocation::~MemoryAllocation(void) noexcept
{
    if (handle())
        parent().memoryAllocator().deallocate(handle());
}

GPU::MemoryAllocation::MemoryAllocation(const MemoryAllocationModel &model) noexcept
{
    handle() = parent().memoryAllocator().allocate(model);
}

std::uint8_t *GPU::MemoryAllocation::beginMemoryMapImpl(void) noexcept
{
    return parent().memoryAllocator().beginMemoryMap<std::uint8_t>(*this);
}

void GPU::MemoryAllocation::endMemoryMap(void) noexcept
{
    return parent().memoryAllocator().endMemoryMap(*this);
}
