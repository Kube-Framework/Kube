/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String utils
 */

#pragma once

#include <ostream>

#include "StringDetails.hpp"

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
inline std::ostream &operator<<(std::ostream &stream, const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &str)
{
    return stream << str.toView();
}

namespace kF::Core
{
    /** @brief Check if a string contains another one without taking case into account */
    [[nodiscard]] inline bool ContainsInsensitiveString(const std::string_view &text, const std::string_view &pattern)
    {
        constexpr auto CaseInsensitiveCharCompare = [](const char a, const char b) { return std::tolower(a) == std::tolower(b); };
        const auto it = std::search(text.begin(), text.end(), pattern.begin(), pattern.end(), CaseInsensitiveCharCompare);
        return it != text.end();
    }
}