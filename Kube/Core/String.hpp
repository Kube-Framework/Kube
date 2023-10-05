/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String
 */

#pragma once

#include "StringDetails.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    /**
     * @brief 16 bytes string that has its size and capacity close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::uint32_t>
    using StringBase = Internal::StringDetails<Vector<Type, Allocator, Range>, Type, Range, false>;

    /**
     * @brief 24 bytes string that has its size and capacity close to the data pointer with a long range
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongStringBase = StringBase<Type, Allocator, std::size_t>;

    /**
     * @brief 16 bytes string using signed char
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using String = StringBase<char, Allocator>;

    /**
     * @brief 24 bytes string using signed char with a long range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongString = LongStringBase<char, Allocator>;

    /**
     * @brief 16 bytes string using signed wchar
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using WString = StringBase<wchar_t, Allocator>;

    /**
     * @brief 24 bytes string using signed wchar with a long range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using LongWString = LongStringBase<wchar_t, Allocator>;
}