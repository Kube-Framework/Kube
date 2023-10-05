/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Kube's literals
 */

#include <gtest/gtest.h>

#include <Kube/Core/Utils.hpp>

using namespace kF;

static_assert(Core::IsMoveIterator<std::move_iterator<int *>>::Value, "IsMoveIterator not working");
static_assert(Core::IsMoveIterator<std::reverse_iterator<std::move_iterator<int *>>>::Value, "IsMoveIterator not working");
static_assert(!Core::IsMoveIterator<std::reverse_iterator<int *>>::Value, "IsMoveIterator not working");
static_assert(!Core::IsMoveIterator<int *>::Value, "IsMoveIterator not working");


enum class Flag : std::uint32_t
{
    A = 0b1,
    B = 0b10,
    AB = 0b11,
    C = 0b100,
    AC = 0b101,
    BC = 0b110,
    ABC = 0b111
};

TEST(Utils, MakeFlags)
{
    ASSERT_EQ(Core::MakeFlags(Flag::A, Flag::B), Flag::AB);
    ASSERT_EQ(Core::MakeFlags(Flag::A, Flag::C), Flag::AC);
    ASSERT_EQ(Core::MakeFlags(Flag::B, Flag::C), Flag::BC);
    ASSERT_EQ(Core::MakeFlags(Flag::A, Flag::B, Flag::C), Flag::ABC);
}

TEST(Utils, HasFlags)
{
    ASSERT_TRUE(Core::HasFlags(Flag::AB, Flag::A));
    ASSERT_TRUE(Core::HasFlags(Flag::AB, Flag::B));
    ASSERT_TRUE(Core::HasFlags(Flag::AB, Flag::A, Flag::B));

    ASSERT_TRUE(Core::HasFlags(Flag::AC, Flag::A));
    ASSERT_TRUE(Core::HasFlags(Flag::AC, Flag::C));
    ASSERT_TRUE(Core::HasFlags(Flag::AC, Flag::A, Flag::C));

    ASSERT_TRUE(Core::HasFlags(Flag::BC, Flag::B));
    ASSERT_TRUE(Core::HasFlags(Flag::BC, Flag::C));
    ASSERT_TRUE(Core::HasFlags(Flag::BC, Flag::B, Flag::C));

    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::A));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::B));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::C));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::AB));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::AC));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::BC));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::A, Flag::B, Flag::C));
    ASSERT_TRUE(Core::HasFlags(Flag::ABC, Flag::AB, Flag::AC, Flag::BC));

    ASSERT_FALSE(Core::HasFlags(Flag::AB, Flag::C));
    ASSERT_FALSE(Core::HasFlags(Flag::AB, Flag::A, Flag::C));
    ASSERT_FALSE(Core::HasFlags(Flag::AB, Flag::A, Flag::B, Flag::C));

    ASSERT_FALSE(Core::HasFlags(Flag::AC, Flag::B));
    ASSERT_FALSE(Core::HasFlags(Flag::AC, Flag::A, Flag::B));
    ASSERT_FALSE(Core::HasFlags(Flag::AC, Flag::A, Flag::B, Flag::C));

    ASSERT_FALSE(Core::HasFlags(Flag::BC, Flag::A));
    ASSERT_FALSE(Core::HasFlags(Flag::BC, Flag::A, Flag::B));
    ASSERT_FALSE(Core::HasFlags(Flag::BC, Flag::A, Flag::B, Flag::C));
}

TEST(Utils, HasAnyFlags)
{
    ASSERT_TRUE(Core::HasAnyFlags(Flag::A, Flag::A));
    ASSERT_TRUE(Core::HasAnyFlags(Flag::A, Flag::A, Flag::B));
    ASSERT_TRUE(Core::HasAnyFlags(Flag::A, Flag::A, Flag::B, Flag::C));

    ASSERT_FALSE(Core::HasAnyFlags(Flag::A, Flag::B));
    ASSERT_FALSE(Core::HasAnyFlags(Flag::A, Flag::C));
    ASSERT_FALSE(Core::HasAnyFlags(Flag::A, Flag::B, Flag::C));
}