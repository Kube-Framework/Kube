/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedAllocatedFlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 16 bytes vector that allocates its size and capacity on the heap
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam CustomHeaderType Custom header of the flat vector
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType, std::integral Range = std::uint32_t>
    using SortedAllocatedFlatVector = Internal::SortedVectorDetails<Internal::AllocatedFlatVectorBase<Type, CustomHeaderType, Range>, Type, Compare, Range, false, true>;

    /** @brief 16 bytes vector that allocates its size and capacity on the heap with a long range
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam CustomHeaderType Custom header of the flat vector
     */
    template<typename Type, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedAllocatedLongFlatVector = SortedAllocatedFlatVector<Type, Compare, CustomHeaderType, std::size_t>;
}