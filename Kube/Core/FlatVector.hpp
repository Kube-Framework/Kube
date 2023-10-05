/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam CustomHeaderType Custom header of the flat vector
     * @tparam Range Range of container
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, typename CustomHeaderType = Internal::NoCustomHeaderType, std::integral Range = std::uint32_t>
    using FlatVector = Internal::VectorDetails<Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>, Type, Range, false, false>;

    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap with a long range
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam CustomHeaderType Custom header of the flat vector
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using LongFlatVector = FlatVector<Type, Allocator, CustomHeaderType, std::size_t>;
}
