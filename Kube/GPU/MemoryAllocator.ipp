/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocator
 */

#include <Kube/Core/Abort.hpp>

inline kF::GPU::MemoryAllocationHandle kF::GPU::MemoryAllocator::allocate(const MemoryAllocationModel &model) noexcept
{
    MemoryAllocationHandle allocation;

    allocate(&model, &model + 1, &allocation, &allocation + 1);
    return allocation;
}

template<typename Type>
inline Type *kF::GPU::MemoryAllocator::beginMemoryMap(const MemoryAllocationHandle allocation) const noexcept
{
    void *target {};
    if (const auto res = ::vmaMapMemory(handle(), allocation, &target); res != VK_SUCCESS)
        kFAbort("GPU::MemoryAllocator::beginMemoryMap: Couldn't map allocation memory '", ErrorMessage(res), '\'');
    return reinterpret_cast<Type *>(target);
}

inline void kF::GPU::MemoryAllocator::endMemoryMap(const MemoryAllocationHandle allocation) const noexcept
{
    ::vmaUnmapMemory(handle(), allocation);
}