/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedFlatString
 */

#pragma once

#include "FlatString.hpp"
#include "AllocatedFlatVector.hpp"

namespace kF::Core
{
    /**
     * @brief 16 bytes string that allocates its size and capacity on the heap
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::uint32_t>
    using AllocatedFlatStringBase = Internal::StringDetails<AllocatedFlatVector<Type, Range>, Type, Range, true>;

    /**
     * @brief 16 bytes string that allocates its size and capacity on the heap with a long range
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     */
    template<typename Type>
    using AllocatedLongFlatStringBase = AllocatedFlatStringBase<Type, std::size_t>;


    /** @brief 16 bytes string that allocates its size and capacity on the heap
     *  The string is non-null terminated */
    using AllocatedFlatString = AllocatedFlatStringBase<char>;

    /** @brief 16 bytes string that allocates its size and capacity on the heap with a long range
     * The string is non-null terminated */
    using AllocatedLongFlatString = AllocatedLongFlatStringBase<char>;


    /** @brief 16 bytes string that allocates its size and capacity on the heap
     *  The string is non-null terminated */
    using AllocatedFlatWString = AllocatedFlatStringBase<wchar_t>;

    /** @brief 16 bytes string that allocates its size and capacity on the heap with a long range
     * The string is non-null terminated */
    using AllocatedLongFlatWString = AllocatedLongFlatStringBase<wchar_t>;
}