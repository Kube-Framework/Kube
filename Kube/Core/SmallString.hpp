/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SmallString
 */

#pragma once

#include "SmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::uint32_t>
    using SmallStringBase = Internal::StringDetails<SmallVector<Type, OptimizedCapacity, Allocator, Range>, Type, Range, false>;

    /**
     * @brief Small optimized string with a long range
     *  The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     */
    template<typename Type, std::size_t OptimizedCapacity, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongSmallStringBase = SmallStringBase<Type, OptimizedCapacity, Allocator, std::size_t>;

    /**
     * @brief 32 bytes small optimized string (cache of 16 bytes)
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SmallString = SmallStringBase<char, CacheLineQuarterSize, Allocator>;

    /**
     * @brief 32 bytes small optimized string (cache of 8 bytes) with a long range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongSmallString = LongSmallStringBase<char, CacheLineEighthSize, Allocator>;

    /**
     * @brief 64 bytes small optimized string (cache of 48 bytes)
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SmallWString = SmallStringBase<wchar_t, (CacheLineHalfSize + CacheLineQuarterSize) / sizeof(wchar_t), Allocator>;

    /**
     * @brief 64 bytes small optimized string (cache of 40 bytes) with a long range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongSmallWString = LongSmallStringBase<wchar_t, (CacheLineHalfSize + CacheLineEighthSize) / sizeof(wchar_t), Allocator>;
}