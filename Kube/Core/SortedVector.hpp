/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Sorted Vector that has its size and capacity close to the data pointer
     * @note With default range (std::uint32_t), the vector takes 16 bytes by default
     * @note The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Compare Compare operator
     * @tparam Range Range of container
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, typename Compare = std::less<Type>, std::integral Range = std::uint32_t>
    using SortedVector = Internal::SortedVectorDetails<Internal::VectorBase<Type, Allocator, Range>, Type, Compare, Range, false, false>;

    /**
     * @brief 24 bytes sorted vector with a long range
     * @note The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Compare Compare operator
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, typename Compare = std::less<Type>>
    using SortedLongVector = SortedVector<Type, Allocator, Compare, std::size_t>;
}