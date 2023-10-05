/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

#pragma once

#include "StringDetails.hpp"
#include "FlatVector.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes string
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::size_t>
    using FlatStringBase = Internal::StringDetails<FlatVector<Type, Allocator, Range>, Type, Range, false>;

    /**
     * @brief 8 bytes string with a long range
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongFlatStringBase = FlatStringBase<Type, Allocator, std::uint32_t>;

    /**
     * @brief 8 bytes string using signed char and size_t range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using FlatString = FlatStringBase<char, Allocator>;

    /**
     * @brief 8 bytes string using signed char with a long range
     * The string is non-null terminated
     */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongFlatString = LongFlatStringBase<char, Allocator>;

    /**
     * @brief 8 bytes string using signed wchar and size_t range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using FlatWString = FlatStringBase<wchar_t, Allocator>;

    /**
     * @brief 8 bytes string using signed wchar with a long range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongFlatWString = LongFlatStringBase<wchar_t, Allocator>;
}
