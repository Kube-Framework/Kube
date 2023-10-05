/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Compile time hash function
 */

#pragma once

#include <string_view>
#include <cstdint>

namespace kF::Core
{
    /** @brief The result type of the hash function */
    using HashedName = std::uint32_t;

    /** @brief The hash begin offset */
    constexpr HashedName HashOffset = 4294967291u;


    /** @brief Compile-time string hashing */
    template<typename Type>
        requires std::is_same_v<Type, char> || std::is_same_v<Type, wchar_t>
    [[nodiscard]] constexpr HashedName ContinueHash(HashedName hash, const Type * const str, const std::size_t len) noexcept
    {
        for (std::size_t i = 0ul; i != len; ++i)
            hash = 31u * hash + static_cast<HashedName>(str[i]);
        return hash;
    }

    /** @brief Compile-time char hashing from a specific hash point */
    template<typename Type>
        requires std::is_same_v<Type, char> || std::is_same_v<Type, wchar_t>
    [[nodiscard]] constexpr HashedName ContinueHash(const HashedName hash, const Type c) noexcept
        { return 31u * hash + static_cast<HashedName>(c); }

    /** @brief Compile-time string-view hashing from a specific hash point */
    [[nodiscard]] constexpr HashedName ContinueHash(const HashedName hash, const std::string_view &str) noexcept
        { return ContinueHash(hash, str.data(), str.length()); }

    /** @brief Compile-time wstring-view hashing from a specific hash point */
    [[nodiscard]] constexpr HashedName ContinueHash(const HashedName hash, const std::wstring_view &str) noexcept
        { return ContinueHash(hash, str.data(), str.length()); }


    /** @brief Compile-time string hashing */
    template<typename Type>
        requires std::is_same_v<Type, char> || std::is_same_v<Type, wchar_t>
    [[nodiscard]] constexpr HashedName Hash(const Type * const str, const std::size_t len) noexcept
        { return ContinueHash(HashOffset, str, len); }

    /** @brief Compile-time char hashing */
    template<typename Type>
        requires std::is_same_v<Type, char> || std::is_same_v<Type, wchar_t>
    [[nodiscard]] constexpr HashedName Hash(const Type c) noexcept
        { return ContinueHash(HashOffset, c); }

    /** @brief Compile-time string-view hashing */
    [[nodiscard]] constexpr HashedName Hash(const std::string_view &str) noexcept
        { return ContinueHash(HashOffset, str.data(), str.length()); }

    /** @brief Compile-time wstring-view hashing */
    [[nodiscard]] constexpr HashedName Hash(const std::wstring_view &str) noexcept
        { return ContinueHash(HashOffset, str.data(), str.length()); }

    namespace Literal
    {
        /** @brief Compile-time string hashing literal */
        [[nodiscard]] constexpr HashedName operator ""_hash(const char *str, const std::size_t len) noexcept { return Hash(str, len); }

        /** @brief Compile-time string hashing literal */
        [[nodiscard]] constexpr HashedName operator ""_hash(const wchar_t *str, const std::size_t len) noexcept { return Hash(str, len); }

        static_assert(""_hash == HashOffset, "There is an error in compile-time hashing algorithm");
    }
    static_assert(Hash("1234") == ContinueHash(ContinueHash(Hash('1'), "2"), "34"), "There is an error in compile-time hashing algorithm");
}
