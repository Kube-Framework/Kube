/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedFlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 16 bytes vector that allocates its size and capacity on the heap
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     * @tparam CustomHeaderType Custom header of the flat vector
     */
    template<typename Type, typename CustomHeaderType = Internal::NoCustomHeaderType, std::integral Range = std::uint32_t>
    using AllocatedFlatVector = Internal::VectorDetails<Internal::AllocatedFlatVectorBase<Type, CustomHeaderType, Range>, Type, Range, false, true>;

    /** @brief 16 bytes vector that allocates its size and capacity on the heap with a long range
     *
     *  @tparam Type Internal type in container
     *  @tparam CustomHeaderType Custom header of the flat vector
     */
    template<typename Type, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using AllocatedLongFlatVector = AllocatedFlatVector<Type, CustomHeaderType, std::size_t>;
}
