/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::uint32_t), the vector takes 16 bytes by default
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::uint32_t>
    using Vector = Internal::VectorDetails<Internal::VectorBase<Type, Allocator, Range>, Type, Range, false, false>;

    /**
     * @brief 24 bytes vector with a long range
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongVector = Vector<Type, Allocator, std::size_t>;
}