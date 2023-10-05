/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedAllocatedVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::uint32_t), the vector takes 24 bytes
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, std::integral Range = std::uint32_t>
    using SortedAllocatedVector = Internal::SortedVectorDetails<Internal::AllocatedVectorBase<Type, Range>, Type, Compare, Range, false, true>;

    /**
     * @brief 32 bytes vector that has its size and capacity close to the data pointer with reduced range
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     */
    template<typename Type, typename Compare = std::less<Type>>
    using SortedAllocatedLongVector = SortedAllocatedVector<Type, Compare, std::size_t>;
}