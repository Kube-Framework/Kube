/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unicode tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/Unicode.hpp>

using namespace kF;

template<typename Type, typename Value>
static void TestValue(const Type &ptr, const Value &value)
{
    ASSERT_TRUE(ptr);
    ASSERT_EQ(ptr, ptr.get());
    ASSERT_EQ(*ptr, value);
    ASSERT_EQ(*ptr.operator->(), value);
    ASSERT_EQ(*ptr.get(), value);
}

TEST(Unicode, Length)
{
    // ASCII
    ASSERT_EQ(Core::Unicode::Length(""), 0);
    ASSERT_EQ(Core::Unicode::Length("1"), 1);
    ASSERT_EQ(Core::Unicode::Length("12"), 2);
    ASSERT_EQ(Core::Unicode::Length("123"), 3);

    // UTF8
    ASSERT_EQ(Core::Unicode::Length("é"), 1);
    ASSERT_EQ(Core::Unicode::Length("한자"), 2);
    ASSERT_EQ(Core::Unicode::Length("é à"), 3);
    ASSERT_EQ(Core::Unicode::Length("😍"), 1);
    ASSERT_EQ(Core::Unicode::Length("😍 😍 😍"), 5);
}

TEST(Unicode, GetNextAndPreviousChar)
{
    const auto Test = [](const std::string_view &utf8, const std::initializer_list<std::uint32_t> &unicodes) -> bool {
        if (Core::Unicode::Length(utf8) != std::size(unicodes))
            return false;
        { // Forward
            auto from = utf8.data();
            auto to = utf8.data() + utf8.size();
            auto count = 0u;
            while (true) {
                const auto unicode = Core::Unicode::GetNextChar(from, to);
                if (!unicode)
                    break;
                if (unicode != *(unicodes.begin() + count))
                    return false;
                ++count;
            }
            if (count != std::size(unicodes))
                return false;
        }
        {
            auto from = utf8.data() + utf8.size();
            auto to = utf8.data();
            auto count = 0u;
            while (true) {
                const auto unicode = Core::Unicode::GetPreviousChar(from, to);
                if (!unicode)
                    break;
                ++count;
                if (unicode != *(unicodes.begin() + (unicodes.size() - count)))
                    return false;
            }
            if (count != std::size(unicodes))
                return false;
        }
        return true;
    };

    ASSERT_TRUE(Test("123", { '1', '2', '3' }));
    ASSERT_TRUE(Test("😍", { 0x1F60Du }));
    ASSERT_TRUE(Test("é à", { 0xE9u, ' ', 0xE0u }));
}