/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::uint32_t), the vector takes 24 bytes
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::uint32_t>
    using AllocatedVector = Internal::VectorDetails<Internal::AllocatedVectorBase<Type, Range>, Type, Range, false, true>;

    /** @brief 32 bytes vector with a long range
     *
     * @tparam Type Internal type in container
     */
    template<typename Type>
    using AllocatedLongVector = AllocatedVector<Type, std::size_t>;
}