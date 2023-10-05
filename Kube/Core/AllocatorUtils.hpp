/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocator utilities
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    namespace AllocatorUtils
    {
        /** @brief Header of an allocation */
        struct AllocationHeader
        {
            AllocationHeader *next {};
        };


        /** @brief Fallback allocate function */
        [[nodiscard]] inline void *FallbackAllocate(const std::size_t size, const std::size_t alignment) noexcept
            { return AlignedAlloc(size, alignment); }

        /** @brief Fallback deallocate function */
        inline void FallbackDeallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept
            { return AlignedFree(data, size, alignment); }


        /** @brief Get the bucket index of a runtime size considering the minimal power size of the allocator (constant time) */
        template<std::size_t MinSizePower>
        [[nodiscard]] std::size_t GetBucketIndex(const std::size_t size) noexcept;

        /** @brief Find the largest bucket size within availableSize
         *  @param head The head allow to track alignment of availableSize
         *  @note The bucket is guaranted to be aligned over its size */
        template<std::size_t MaxSizePower>
        [[nodiscard]] std::size_t FindBucketFit(const std::size_t availableSize, const std::size_t head) noexcept;


        /** @brief Get the ideal stack size of allocator using a target bucket size and a page size */
        template<std::size_t StackMetaDataSize, std::size_t MaxStackSize>
        [[nodiscard]] std::size_t GetStackSize(const std::size_t bucketSize, const std::size_t pageSize, const std::size_t lastStackSize) noexcept;
    }
}

#include "AllocatorUtils.ipp"