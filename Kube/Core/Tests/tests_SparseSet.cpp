/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial functor unit tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/SparseSet.hpp>

using namespace kF;

struct Countable
{
    static inline int Counter = 0;

    int value {};

    Countable(const int v) noexcept : value(v) { ++Counter; }
    ~Countable(void) noexcept { --Counter; }
};

TEST(SparseSet, Basics)
{
    constexpr auto PageSize = 1024;
    constexpr auto Index1 = 42;
    constexpr auto Index2 = PageSize + Index1;
    constexpr auto Value1 = 42;
    constexpr auto Value2 = 24;

    Countable::Counter = 0;

    Core::SparseSet<Countable, PageSize> sparseSet;

    ASSERT_EQ(Countable::Counter, 0);

    ASSERT_EQ(sparseSet.add(Index1, Value1).value, Value1);
    ASSERT_EQ(sparseSet.at(Index1).value, Value1);

    ASSERT_EQ(Countable::Counter, 1);
    sparseSet.remove(Index1);
    ASSERT_EQ(Countable::Counter, 0);

    ASSERT_EQ(sparseSet.add(Index2, Value2).value, Value2);
    ASSERT_EQ(sparseSet.at(Index2).value, Value2);

    // Opps we did not removed index !
    sparseSet.releaseUnsafe();

    // Consequence is non-destructed object (=> memory leak)
    ASSERT_EQ(Countable::Counter, 1);
}