/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unique pointer tests
 */

#include <string>

#include <gtest/gtest.h>

#include <Kube/Core/UniquePtr.hpp>

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

TEST(UniquePtr, Int)
{
    constexpr int Value1 = 42;
    constexpr int Value2 = 24;

    // Make
    auto ptr1 = Core::UniquePtr<int>::Make(Value1);
    TestValue(ptr1, Value1);

    // Move constructor
    Core::UniquePtr<int> ptr2(std::move(ptr1));
    TestValue(ptr2, Value1);
    *ptr2 = Value2;
    TestValue(ptr2, Value2);

    // Swap && move assignment
    Core::UniquePtr<int> ptr3;
    ptr3.swap(ptr1);
    ASSERT_FALSE(ptr3);
    ptr3 = std::move(ptr2);
    TestValue(ptr3, Value2);
}

TEST(UniquePtr, String)
{
    const std::string Value1 = "Kube";
    const std::string Value2 = "abcdefghijklmnopqrstuvwxyz";

    // Make
    auto ptr1 = Core::UniquePtr<std::string>::Make(Value1);
    TestValue(ptr1, Value1);

    // Move constructor
    Core::UniquePtr<std::string> ptr2(std::move(ptr1));
    TestValue(ptr2, Value1);
    *ptr2 = Value2;
    TestValue(ptr2, Value2);

    // Swap && move assignment
    Core::UniquePtr<std::string> ptr3;
    ptr3.swap(ptr1);
    ASSERT_FALSE(ptr3);
    ptr3 = std::move(ptr2);
    TestValue(ptr3, Value2);
}