/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocator
 */

#pragma once

#include "Handle.hpp"
#include "MemoryAllocationModel.hpp"

namespace kF::GPU
{
    class MemoryAllocator;
}

class kF::GPU::MemoryAllocator : public CachedHandle<VmaAllocator>
{
public:
    /** @brief Destruct the buffer */
    ~MemoryAllocator(void) noexcept;

    /** @brief Construct a new memory allocator using VMA */
    MemoryAllocator(void) noexcept;

    /** @brief Move constructor */
    MemoryAllocator(MemoryAllocator &&other) noexcept = default;

    /** @brief Move assignment */
    MemoryAllocator &operator=(MemoryAllocator &&other) noexcept = default;


    /** @brief Allocate memory */
    [[nodiscard]] MemoryAllocationHandle allocate(const MemoryAllocationModel &model) noexcept;

    /** @brief Allocate a list of memory */
    void allocate(const MemoryAllocationModel * const modelFrom, const MemoryAllocationModel * const modelTo,
            MemoryAllocationHandle * const allocationFrom, MemoryAllocationHandle * const allocationTo) noexcept;

    /** @brief Deallocate memory */
    void deallocate(const MemoryAllocationHandle allocation) noexcept
        { deallocate(&allocation, &allocation + 1); }

    /** @brief Deallocate a list of memory */
    void deallocate(const MemoryAllocationHandle * const allocationFrom, const MemoryAllocationHandle * const allocationTo) noexcept;


    /** @brief Map the memory of an allocation */
    template<typename Type>
    [[nodiscard]] Type *beginMemoryMap(const MemoryAllocationHandle allocation) const noexcept;

    /** @brief Unmap the memory of an allocation */
    void endMemoryMap(const MemoryAllocationHandle allocation) const noexcept;
};

#include "MemoryAllocator.ipp"