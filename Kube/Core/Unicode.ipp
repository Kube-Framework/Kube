#include "Unicode.hpp"

using namespace kF;

template<typename Range>
constexpr Range kF::Core::Unicode::Length(const std::string_view &utf8) noexcept
{
    Range unicodeLength {};
    for (std::size_t index {}, count = utf8.size(); index != count; ++index, ++unicodeLength) {
        if (const auto c = reinterpret_cast<const std::uint8_t &>(utf8[index]); c >= 0b10000000u) {
            index += std::min<Range>(
                (c >= 0b11000000u) + (c >= 0b11100000u) + (c >= 0b11110000u),
                Range(count - index)
            );
        }
    }
    return unicodeLength;
}

template<typename Iterator>
constexpr std::uint32_t kF::Core::Unicode::GetNextCharByteCount(const Iterator from, const Iterator end) noexcept
{
    if (from == end)
        return 0u;
    const auto c = std::uint32_t(reinterpret_cast<const std::uint8_t &>(*from));
    return std::min<std::uint32_t>(
        1u + std::uint32_t(c >= 0b11000000u) + std::uint32_t(c >= 0b11100000u) + std::uint32_t(c >= 0b11110000u),
        Core::Distance<std::uint32_t>(from, end)
    );
}

template<typename Iterator>
constexpr std::uint32_t kF::Core::Unicode::GetPreviousCharByteCount(const Iterator from, const Iterator begin) noexcept
{
    auto it = from;
    while (it != begin && (std::uint32_t(reinterpret_cast<const std::uint8_t &>(*--it)) & 0b11000000u) == 0b10000000u);
    return Core::Distance<std::uint32_t>(it, from);
}

template<typename Iterator>
constexpr std::uint32_t kF::Core::Unicode::GetNextChar(Iterator &from, const Iterator end) noexcept
{
    const auto byteCount = GetNextCharByteCount(from, end);
    const auto unicode = Decode(from, byteCount);
    from += byteCount;
    return unicode;
}

template<typename Iterator>
constexpr std::uint32_t kF::Core::Unicode::GetPreviousChar(Iterator &from, const Iterator begin) noexcept
{
    const auto byteCount = GetPreviousCharByteCount(from, begin);
    from -= byteCount;
    return Decode(from, byteCount);
}

template<typename Iterator>
constexpr std::uint32_t kF::Core::Unicode::Decode(const Iterator data, const std::uint32_t byteCount) noexcept
{
    if (!byteCount)
        return 0u;
    const auto c = std::uint32_t(reinterpret_cast<const std::uint8_t &>(*data));
    if (byteCount == 1u)
        return c;
    auto count = byteCount;
    auto it = data + 1;
    auto res = (c & (0b11111111u >> (count + 1u))) << 6u * (count - 1);
    while (--count) {
        const auto c2 = (std::uint32_t(reinterpret_cast<const std::uint8_t &>(*it)) & 0b00111111u) << 6u * (count - 1);
        res |= c2;
        ++it;
    }
    return res;
}
