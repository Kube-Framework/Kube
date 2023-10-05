/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedSmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * @note The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     * @tparam Compare Compare operator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
        typename Compare = std::less<Type>, std::integral Range = std::uint32_t>
    using SortedSmallVector = Internal::SortedVectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>, Type, Compare, Range, true, false>;

    /**
     * @brief Small optimized vector with a long range
     * @note The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Compare Compare operator*/
    template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
        typename Compare = std::less<Type>>
    using SortedLongSmallVector = SortedSmallVector<Type, OptimizedCapacity, Allocator, Compare, std::size_t>;
}