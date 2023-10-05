/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Random number generator
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Random
{
    /** @brief Generate a random 32 bytes unsigned integer
     *  This function is thread safe */
    [[nodiscard]] std::uint32_t Generate32(void) noexcept;

    /** @brief Generate a random 32 bytes unsigned integer in range [0, to[
     *  This function is thread safe */
    [[nodiscard]] std::uint32_t Generate32(const std::uint32_t to) noexcept;


    /** @brief Generate a random 64 bytes unsigned integer
     *  This function is thread safe */
    [[nodiscard]] std::uint64_t Generate64(void) noexcept;

    /** @brief Generate a random 64 bytes unsigned integer in range [0, to[
     *  This function is thread safe */
    [[nodiscard]] std::uint64_t Generate64(const std::uint64_t to) noexcept;
}
