#pragma once

#include "Utils.hpp"

#include <string_view>

namespace kF::Core::Unicode
{
    /** @brief Get the length on an unicode string from an utf8 one */
    template<typename Range = std::size_t>
    [[nodiscard]] constexpr Range Length(const std::string_view &utf8) noexcept;

    /** @brief Get the byte count of the next unicode character */
    template<typename Iterator>
    [[nodiscard]] constexpr std::uint32_t GetNextCharByteCount(const Iterator it, const Iterator end) noexcept;

    /** @brief Get the byte count of the previous unicode character */
    template<typename Iterator>
    [[nodiscard]] constexpr std::uint32_t GetPreviousCharByteCount(const Iterator it, const Iterator begin) noexcept;

    /** @brief Get the next unicode character in utf8 string */
    template<typename Iterator>
    [[nodiscard]] constexpr std::uint32_t GetNextChar(Iterator &from, const Iterator end) noexcept;

    /** @brief Get the previous unicode character in utf8 string */
    template<typename Iterator>
    [[nodiscard]] constexpr std::uint32_t GetPreviousChar(Iterator &from, const Iterator begin) noexcept;

    /** @brief Decode an utf8 character */
    template<typename Iterator>
    [[nodiscard]] constexpr std::uint32_t Decode(const Iterator data, const std::uint32_t byteCount) noexcept;
}

#include "Unicode.ipp"