/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FixedString
 */

#include <algorithm>

#include "FixedString.hpp"

template<typename CharType, std::size_t N>
constexpr kF::Core::FixedStringBase<CharType, N>::FixedStringBase(const CharType (&array)[N]) noexcept
{
    for (std::size_t i = 0; i != N; ++i)
        cache[i] = array[i];
}

template<typename CharType, std::size_t N>
constexpr kF::Core::FixedStringBase<CharType, N> &kF::Core::FixedStringBase<CharType, N>::operator=(const CharType (&array)[N]) noexcept
{
    for (std::size_t i = 0; i != N; ++i)
        cache[i] = array[i];
    return *this;
}

template<typename CharType, std::size_t L, std::size_t R>
[[nodiscard]] constexpr kF::Core::FixedStringBase<CharType, L + R> kF::Core::operator+(const FixedStringBase<CharType, L> &lhs,
        const FixedStringBase<CharType, R> &rhs) noexcept
{
    kF::Core::FixedStringBase<CharType, L + R> res;
    auto it = std::copy_n(lhs.begin(), L, res.begin());
    std::copy_n(rhs.begin(), R, it);
    return res;
}

template<typename CharType, std::size_t L, std::size_t R>
[[nodiscard]] constexpr kF::Core::FixedStringBase<CharType, L + R> kF::Core::operator+(const FixedStringBase<CharType, L> &lhs,
    const CharType(&rhs)[R]) noexcept
{
    kF::Core::FixedStringBase<CharType, L + R> res;
    auto it = std::copy_n(lhs.begin(), L, res.begin());
    std::copy_n(rhs, R, it);
    return res;
}

template<typename CharType, std::size_t L, std::size_t R>
[[nodiscard]] constexpr kF::Core::FixedStringBase<CharType, L + R> kF::Core::operator+(const CharType(&lhs)[L],
        const FixedStringBase<CharType, R> &rhs) noexcept
{
    kF::Core::FixedStringBase<CharType, L + R> res;
    auto it = std::copy_n(lhs, L, res.begin());
    std::copy_n(rhs.begin(), R, it);
    return res;
}

template<typename CharType, std::size_t N>
[[nodiscard]] constexpr kF::Core::FixedStringBase<CharType, N + 1> kF::Core::operator+(const CharType lhs, const FixedStringBase<CharType, N> &rhs) noexcept
{
    kF::Core::FixedStringBase<CharType, N + 1> res;
    res[0] = lhs;
    std::copy_n(res.begin(), rhs.begin(), N);
    return res;
}

template<typename CharType, std::size_t N>
[[nodiscard]] constexpr kF::Core::FixedStringBase<CharType, N + 1> kF::Core::operator+(const FixedStringBase<CharType, N> &lhs, const CharType rhs) noexcept
{
    kF::Core::FixedStringBase<CharType, N + 1> res;
    std::copy_n(res.begin(), lhs.begin(), N);
    res[N] = rhs;
    return res;
}
