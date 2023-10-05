/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FixedString
 */

#pragma once

#include <cstring>
#include <compare>
#include <string_view>

namespace kF::Core
{
    // Class template FixedStringBase
    template<typename CharType, std::size_t N>
    struct FixedStringBase;


    /** @brief Creation helper */
    template<class CharType, std::size_t N>
    constexpr FixedStringBase<CharType, N> MakeFixedString(const CharType(&array)[N]) noexcept
        { return FixedStringBase<CharType, N>(array); }


    /** @brief FixedString using 8 bit characters */
    template<std::size_t N>
    struct FixedString : FixedStringBase<char, N>
    {
        using FixedStringBase<char, N>::FixedStringBase;
    };

    /** @brief FixedString deduction guide */
    template<std::size_t N>
    FixedString(const char (&)[N]) -> FixedString<N>;


    /** @brief FixedString using 8 bit characters */
    template<std::size_t N>
    struct FixedWString : FixedStringBase<wchar_t, N>
    {
        using FixedStringBase<wchar_t, N>::FixedStringBase;
    };

    /** @brief FixedString deduction guide */
    template<std::size_t N>
    FixedWString(const wchar_t (&)[N]) -> FixedWString<N>;


    namespace Internal
    {
        /** @brief Tagged type checker
         *  @note Can detect types that are or derive from a template class with only FixedString as arguments */
        template<class Derived, template<FixedString...> class Base>
        class TagDetector
        {
            /** @brief Success case */
            template<FixedString ...Names>
                requires std::same_as<Derived, Base<Names...>> || std::derived_from<Derived, Base<Names...>>
            static std::true_type test(const Base<Names...> &) noexcept;

            /** @brief Failure case */
            static std::false_type test(...) noexcept;

        public:
            /** @brief Extract final value from detector */
            static constexpr bool Value = decltype(TagDetector::test(std::declval<Derived>()))::value;
        };
    }

    /** @brief Utility tag checker
     *  @note A tag is template structure that only has FixedString as arguments */
    template<typename TagType, template<FixedString...> class Base>
    constexpr bool IsTag = Internal::TagDetector<TagType, Base>::Value;

    namespace Literal
    {
        /** @brief Fixed string literal */
        template<FixedStringBase Str>
        [[nodiscard]] consteval auto operator""_fixed(void) noexcept { return Str; }
    }
}

/** @brief A constexpr fixed string */
template<typename CharType, std::size_t N>
struct kF::Core::FixedStringBase
{
    static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>, "FixedStringBase only supports char & wchar");

    /** @brief Iterator detectors */
    using ValueType = CharType;

    /** @brief Iterator detectors */
    using Iterator = ValueType *;
    using ConstIterator = const ValueType *;

    /** @brief Iterators helper types */
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    /** @brief View */
    using View = std::basic_string_view<CharType>;


    /** @brief Number of characters (except end character) */
    static constexpr auto Capacity = N;

    /** @brief End character */
    static constexpr auto EndCharacter = static_cast<CharType>('\0');


    /** @brief String container member */
    CharType cache[N] {}; // + 1 is for terminating character


    /** @brief Default constructor */
    constexpr FixedStringBase(void) noexcept = default;

    /** @brief Copy constructor */
    // constexpr FixedStringBase(const FixedStringBase &str) noexcept = default; // Ambigous on gcc

    /** @brief Copy array constructor */
    constexpr FixedStringBase(const CharType (&array)[N]) noexcept;

    /** @brief Copy assignment */
    constexpr FixedStringBase &operator=(const FixedStringBase &str) noexcept = default;

    /** @brief Copy array assignment */
    constexpr FixedStringBase &operator=(const CharType (&array)[N]) noexcept;


    /** @brief Generate default comparison functions */
    [[nodiscard]] constexpr auto operator<=>(const FixedStringBase &) const noexcept = default;


    /** @brief Begin / end overloads */
    [[nodiscard]] constexpr Iterator begin(void) noexcept { return cache; }
    [[nodiscard]] constexpr Iterator end(void) noexcept { return cache + N; }
    [[nodiscard]] constexpr ConstIterator begin(void) const noexcept { return cache; }
    [[nodiscard]] constexpr ConstIterator end(void) const noexcept { return cache + N; }
    [[nodiscard]] constexpr ReverseIterator rbegin(void) noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] constexpr ReverseIterator rend(void) noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] constexpr ConstReverseIterator rbegin(void) const noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] constexpr ConstReverseIterator rend(void) const noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] constexpr ConstIterator cbegin(void) const noexcept { return begin(); }
    [[nodiscard]] constexpr ConstIterator cend(void) const noexcept { return end(); }
    [[nodiscard]] constexpr ConstReverseIterator crbegin(void) const noexcept { return rbegin(); }
    [[nodiscard]] constexpr ConstReverseIterator crend(void) const noexcept { return rend(); }


    /** @brief Get the size of the string */
    [[nodiscard]] constexpr std::size_t size(void) const noexcept { return N; }
    [[nodiscard]] constexpr std::size_t length(void) const noexcept { return N; }

    /** @brief Get the capacity of the string */
    [[nodiscard]] constexpr std::size_t capacity(void) const noexcept { return N; }

    /** @brief Empty check */
    [[nodiscard]] constexpr bool empty(void) const noexcept { return cache[0] == EndCharacter; }


    /** @brief Access element at positon */
    [[nodiscard]] constexpr CharType &operator[](const std::size_t pos) noexcept { return cache[pos]; }
    [[nodiscard]] constexpr const CharType &operator[](const std::size_t pos) const noexcept { return cache[pos]; }

    /** @brief Access element at positon */
    [[nodiscard]] constexpr CharType &at(const std::size_t pos) noexcept { return cache[pos]; }
    [[nodiscard]] constexpr const CharType &at(const std::size_t pos) const noexcept { return cache[pos]; }

    /** @brief Get string data */
    [[nodiscard]] constexpr CharType *data(void) noexcept { return cache; }
    [[nodiscard]] constexpr const CharType *data(void) const noexcept { return cache; }

    /** @brief Get string data */
    [[nodiscard]] constexpr View toView(void) const noexcept
        { return View(cache, N - (cache[N - 1] == EndCharacter)); }
};

// Global operators
namespace kF::Core
{
    /** @brief Concatenation of Fixed[L] + Fixed[R] */
    template<typename CharType, std::size_t L, std::size_t R>
    [[nodiscard]] constexpr FixedStringBase<CharType, L + R> operator+(const FixedStringBase<CharType, L> &lhs,
            const FixedStringBase<CharType, R> &rhs) noexcept;

    /** @brief Concatenation of Fixed[L] + CharType[R] */
    template<typename CharType, std::size_t L, std::size_t R>
    [[nodiscard]] constexpr FixedStringBase<CharType, L + R> operator+(const FixedStringBase<CharType, L> &lhs,
        const CharType(&rhs)[R]) noexcept;

    /** @brief Concatenation of CharType[L] + Fixed[R] */
    template<typename CharType, std::size_t L, std::size_t R>
    [[nodiscard]] constexpr FixedStringBase<CharType, L + R> operator+(const CharType(&lhs)[L],
            const FixedStringBase<CharType, R> &rhs) noexcept;

    /** @brief Concatenation of CharType + Fixed[N] */
    template<typename CharType, std::size_t N>
    [[nodiscard]] constexpr FixedStringBase<CharType, N + 1> operator+(const CharType lhs, const FixedStringBase<CharType, N> &rhs) noexcept;

    /** @brief Concatenation of Fixed[N] + CharType */
    template<typename CharType, std::size_t N>
    [[nodiscard]] constexpr FixedStringBase<CharType, N + 1> operator+(const FixedStringBase<CharType, N> &lhs, const CharType rhs) noexcept;
}

#include "FixedString.ipp"