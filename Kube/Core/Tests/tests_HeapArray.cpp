/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#include <Kube/Core/HeapArray.hpp>

using namespace kF;

TEST(HeapArray, Basics)
{
    constexpr auto count = 42ul;
    constexpr auto str = "HeapArray is an amazing fixed array !";
    Core::HeapArray<std::string> array(count, str);

    ASSERT_TRUE(array);
    ASSERT_EQ(array.size(), count);
    auto i = 0ul;
    for (const auto &elem : array) {
        ASSERT_EQ(elem, str);
        ++i;
    }
    ASSERT_EQ(i, count);
    array.release();
    ASSERT_FALSE(array);
    ASSERT_EQ(array.size(), 0);
    array.allocate(count * 2, str);
    ASSERT_TRUE(array);
    ASSERT_EQ(array.size(), count * 2);
    i = 0ul;
    for (const auto &elem : array) {
        ASSERT_EQ(elem, str);
        ++i;
    }
    ASSERT_EQ(i, count * 2);
    array.release();
    ASSERT_EQ(array.size(), 0);
}

TEST(HeapArray, Semantics)
{
    constexpr auto count = 42ul;
    constexpr auto value = 42ul;
    Core::HeapArray<std::size_t> arrayTmp(count, value), arrayTmp2(std::move(arrayTmp));
    Core::HeapArray<std::size_t> array;

    array = std::move(arrayTmp2);
    ASSERT_EQ(array.size(), count);
    auto i = 0ul;
    for (const auto &elem : array) {
        ASSERT_EQ(elem, value);
        ++i;
    }
    ASSERT_EQ(i, count);
}