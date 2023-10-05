/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of maths utility
 */

#pragma once

#include <cmath>
#include <limits>

#include "Utils.hpp"

namespace kF::Core
{
    /** @brief Compute power of number - floating points */
    template<typename Type>
        requires std::is_floating_point_v<Type>
    [[nodiscard]] constexpr Type Pow(const Type x, const Type y) noexcept
        { return std::pow(x, y); }

    /** @brief Compute power of number - integers */
    template<typename Type>
        requires std::is_integral_v<Type>
    [[nodiscard]] constexpr Type Pow(const Type x, const Type y) noexcept;
}

#include "Maths.ipp"