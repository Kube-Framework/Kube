/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedSmallString
 */

#pragma once

#include "AllocatedSmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::uint32_t>
    using AllocatedSmallStringBase = Internal::StringDetails<AllocatedSmallVector<Type, OptimizedCapacity, Range>, Type, Range, true>;

    /**
     * @brief Small optimized string with a long range
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    template<typename Type, std::size_t OptimizedCapacity>
    using AllocatedLongSmallStringBase = AllocatedSmallStringBase<Type, OptimizedCapacity, std::size_t>;


    /** @brief 64 bytes small optimized string (default cache of 40 bytes / 40 chars)
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedSmallString = AllocatedSmallStringBase<char, CacheLineHalfSize + CacheLineEighthSize>;

    /** @brief 64 bytes small optimized string (default cache of 32 bytes / 32 chars) with a long range
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedLongSmallString = AllocatedLongSmallStringBase<char, CacheLineHalfSize>;


    /** @brief 64 bytes small optimized string (default cache of 40 bytes / 20 chars)
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedSmallWString = AllocatedSmallStringBase<wchar_t, (CacheLineHalfSize + CacheLineEighthSize) / sizeof(wchar_t)>;

    /** @brief 64 bytes small optimized string (default cache of 32 bytes / 16 chars) with a long range
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedLongSmallWString = AllocatedLongSmallStringBase<wchar_t, CacheLineHalfSize / sizeof(wchar_t)>;
}
