/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedString
 */

#pragma once

#include "StringDetails.hpp"
#include "AllocatedVector.hpp"

namespace kF::Core
{
    /**
     * @brief 24 bytes string that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::uint32_t>
    using AllocatedStringBase = Internal::StringDetails<AllocatedVector<Type, Range>, Type, Range, true>;

    /**
     * @brief 32 bytes string that has its size, capacity and a small cache close to the data pointer with a long range
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     */
    template<typename Type>
    using AllocatedLongStringBase = AllocatedStringBase<Type, std::size_t>;

    /** @brief 24 bytes string that has its size, capacity and a small cache close to the data pointer
     *  The string is non-null terminated */
    using AllocatedString = AllocatedStringBase<char>;

    /** @brief 32 bytes string that has its size, capacity and a small cache close to the data pointer with a long range
     *  The string is non-null terminated */
    using AllocatedLongString = AllocatedLongStringBase<char>;

    /** @brief 24 bytes string that has its size, capacity and a small cache close to the data pointer
     *  The string is non-null terminated */
    using AllocatedWString = AllocatedStringBase<wchar_t>;

    /** @brief 32 bytes string that has its size, capacity and a small cache close to the data pointer with a long range
     *  The string is non-null terminated */
    using AllocatedLongWString = AllocatedLongStringBase<wchar_t>;
}