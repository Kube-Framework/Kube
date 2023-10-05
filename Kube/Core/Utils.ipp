/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Utils
 */

#include "Utils.hpp"

template<std::integral Range>
constexpr Range kF::Core::AlignPowerOf2(const Range offset, const Range alignment) noexcept
{
    const auto skip = offset & (alignment - 1);
    return offset + Core::BranchlessIf(skip, alignment - skip, Range(0));
}

template<std::integral Range>
constexpr Range kF::Core::AlignNonPowerOf2(const Range offset, const Range alignment) noexcept
{
    const auto mod = offset % alignment;
    return offset + Core::BranchlessIf(mod, alignment - mod, Range(0));
}

template<std::integral Unit>
constexpr Unit kF::Core::IsPowerOf2(Unit value) noexcept
{
    return value && !(value & (value - static_cast<Unit>(1)));
}

template<std::integral Unit>
constexpr Unit kF::Core::NextPowerOf2(Unit value) noexcept
{
    if (value && !(value & (value - static_cast<Unit>(1))))
        return value;
    auto count = static_cast<Unit>(0);
    while (value != 0) {
        value >>= 1;
        ++count;
    }
    return static_cast<Unit>(1) << count;
}

template<std::integral Unit, std::integral ResultUnit>
constexpr ResultUnit kF::Core::NextPowerOf2Bit(Unit value) noexcept
{
    auto count = static_cast<Unit>(0);
    value -= value && !(value & (value - static_cast<Unit>(1))); // Minus 1 when already a power of 2
    while (value != 0) {
        value >>= 1;
        ++count;
    }
    return count;
}

template<typename Type>
    requires std::integral<Type> || std::floating_point<Type>
constexpr Type kF::Core::BranchlessIf(const bool condition, const Type lhs, const Type rhs) noexcept
{
    const auto typeCondition = static_cast<Type>(condition);
    return static_cast<Type>(typeCondition * lhs - rhs * (typeCondition - static_cast<Type>(1)));
}