/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of maths utility
 */

#include "Maths.hpp"

template<typename Type>
    requires std::is_integral_v<Type>
constexpr Type kF::Core::Pow(const Type x, const Type y) noexcept
{
    constexpr auto BitCount = static_cast<Type>(sizeof(Type) * 8);

    Type actual = x;
    Type exp = y;
    Type prod = 1;

    for (Type i {}; i < BitCount; i++) {
        if (exp & 0x1)
            prod *= actual;
        exp >>= 1;
        actual *= actual;
    }
    return prod;
}