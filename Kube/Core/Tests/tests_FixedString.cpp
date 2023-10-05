/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#include <Kube/Core/FixedString.hpp>

using namespace kF;
using namespace kF::Core::Literal;

namespace FixedStringConstexprBasics
{
    constexpr auto Str = Core::MakeFixedString("Hello");
    constexpr auto Str2 = "Hello"_fixed;
    constexpr std::string_view StdStr = "Hello";

    static_assert(Str == Str2);
    static_assert(Str.toView().size() == 5);
    static_assert(StdStr.size() == 5);
    static_assert(Str.toView() == Str2.toView());
    static_assert(Str.toView() == StdStr);
    static_assert(Str2.toView() == StdStr);
}

namespace FixedStringConstexprAdvanced
{
    template<auto Name>
    class Base
    {
    public:
        [[nodiscard]] constexpr auto className(void) const noexcept { return Name.toView(); }

    private:
    };

    using DerivedA = Base<"DerivedA"_fixed>;
    using DerivedB = Base<"DerivedB"_fixed>;
    using DerivedAliasA = Base<"DerivedA"_fixed>;
    using DerivedAliasB = Base<"DerivedB"_fixed>;

    static_assert(std::is_same_v<DerivedA, DerivedAliasA>);
    static_assert(std::is_same_v<DerivedB, DerivedAliasB>);
    static_assert(DerivedA().className() == "DerivedA");
    static_assert(DerivedAliasA().className() == "DerivedA");
    static_assert(DerivedB().className() == "DerivedB");
    static_assert(DerivedAliasB().className() == "DerivedB");
}

TEST(FixedString, Basics)
{
    constexpr char Array[3] = { 'A', 'B' };
    constexpr std::size_t ExpectedSize = 3ul;

    auto str = Core::MakeFixedString("AB");
    auto str2 = Core::MakeFixedString(Array);
    auto str3 = "AB"_fixed;
    // str == str2 && str2 == str3
    ASSERT_EQ(str, str3);   ASSERT_EQ(str2, str3);
    // All lengths are constant
    ASSERT_EQ(str.size(), ExpectedSize);
    ASSERT_EQ(str.length(), ExpectedSize);
    ASSERT_EQ(str.capacity(), ExpectedSize);
    // str <= str2 && str2 >= str3
    ASSERT_LE(str, str2);   ASSERT_GE(str2, str3);

    // str => "CB"
    str[0] = 'C';
    // str != str2 && str != str3
    ASSERT_NE(str, str2);   ASSERT_NE(str, str3);
    // str > str2 && str3 < str
    ASSERT_GT(str, str2);   ASSERT_LT(str3, str);
    // str2 <= str && str >= str3
    ASSERT_LE(str2, str);   ASSERT_GE(str, str3);

    // str2 => "ABC"
    str2.at(2) = 'C';
    // str2 != str && str2 != str3
    ASSERT_NE(str2, str);   ASSERT_NE(str2, str3);
    // str2 > str3 && str2 < str
    ASSERT_GT(str2, str3);  ASSERT_LT(str2, str);
    // str2 <= str && str2 >= str3
    ASSERT_LE(str2, str);   ASSERT_GE(str2, str3);

    // str3 => "DEF"
    for (char i = 0; auto &c : str3)
        c = 'D' + i++;
    // str3 != str && str3 != str2
    ASSERT_NE(str3, str);   ASSERT_NE(str3, str2);
    // str3 > str && str2 < str3
    ASSERT_GT(str3, str);   ASSERT_LT(str2, str3);
    // str <= str3 && str3 >= str2
    ASSERT_LE(str, str3);   ASSERT_GE(str3, str2);

    // All lengths are constant
    ASSERT_EQ(str.size(), ExpectedSize);
    ASSERT_EQ(str.length(), ExpectedSize);
    ASSERT_EQ(str.capacity(), ExpectedSize);
    // Representations
    ASSERT_EQ(str.toView(), std::string_view("CB"));
    ASSERT_EQ(str2.toView(), std::string_view("ABC"));
    ASSERT_EQ(str3.toView(), std::string_view("DEF"));
}