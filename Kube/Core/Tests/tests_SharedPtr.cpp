/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Shared pointer tests
 */

#include <string>

#include <gtest/gtest.h>

#include <Kube/Core/SharedPtr.hpp>

using namespace kF;

template<typename Type, typename Value>
static void TestValue(const Type &ptr, const Value &value)
{
    ASSERT_TRUE(ptr);
    ASSERT_EQ(*ptr, value);
    ASSERT_EQ(*ptr.operator->(), value);
    ASSERT_EQ(*ptr.get(), value);
}

TEST(SharedPtr, Int)
{
    constexpr int Value1 = 42;
    constexpr int Value2 = 24;

    // Make
    auto ptr1 = Core::SharedPtr<int>::Make(Value1);
    TestValue(ptr1, Value1);

    // Move constructor
    Core::SharedPtr<int> ptr2(std::move(ptr1));
    TestValue(ptr2, Value1);
    *ptr2 = Value2;
    TestValue(ptr2, Value2);

    // Swap && move assignment
    Core::SharedPtr<int> ptr3;
    ptr3.swap(ptr1);
    ASSERT_FALSE(ptr3);
    ptr3 = std::move(ptr2);
    TestValue(ptr3, Value2);

    // Copy constructor
    Core::SharedPtr<int> ptr4(ptr3);
    ASSERT_TRUE(ptr4);
    ASSERT_EQ(ptr4.referenceCount(), 2);
    *ptr3 = Value1;
    TestValue(ptr4, Value1);
    ptr4.swap(ptr1);
    ASSERT_FALSE(ptr4);
    ASSERT_EQ(ptr1.referenceCount(), 2);

    // Copy assignment
    ptr4 = ptr1;
    ASSERT_EQ(ptr1.referenceCount(), 3);
    *ptr3 = Value2;
    ASSERT_TRUE(ptr4);
    TestValue(ptr4, Value2);

    // Release reference count
    ptr4.release();
    ASSERT_EQ(ptr1.referenceCount(), 2);
    ptr3.release();
    ASSERT_EQ(ptr1.referenceCount(), 1);
    ptr1.release();
    ASSERT_FALSE(ptr1);
}

TEST(SharedPtr, String)
{
    const std::string Value1 = "Kube";
    const std::string Value2 = "abcdefghijklmnopqrstuvwxyz";

    // Make
    auto ptr1 = Core::SharedPtr<std::string>::Make(Value1);
    TestValue(ptr1, Value1);

    // Move constructor
    Core::SharedPtr<std::string> ptr2(std::move(ptr1));
    TestValue(ptr2, Value1);
    *ptr2 = Value2;
    TestValue(ptr2, Value2);

    // Swap && move assignment
    Core::SharedPtr<std::string> ptr3;
    ptr3.swap(ptr1);
    ASSERT_FALSE(ptr3);
    ptr3 = std::move(ptr2);
    TestValue(ptr3, Value2);

    // Copy constructor
    Core::SharedPtr<std::string> ptr4(ptr3);
    ASSERT_TRUE(ptr4);
    ASSERT_EQ(ptr4.referenceCount(), 2);
    *ptr3 = Value1;
    TestValue(ptr4, Value1);
    ptr4.swap(ptr1);
    ASSERT_FALSE(ptr4);
    ASSERT_EQ(ptr1.referenceCount(), 2);

    // Copy assignment
    ptr4 = ptr1;
    ASSERT_EQ(ptr1.referenceCount(), 3);
    *ptr3 = Value2;
    ASSERT_TRUE(ptr4);
    TestValue(ptr4, Value2);

    // Release reference count
    ptr4.release();
    ASSERT_EQ(ptr1.referenceCount(), 2);
    ptr3.release();
    ASSERT_EQ(ptr1.referenceCount(), 1);
    ptr1.release();
    ASSERT_FALSE(ptr1);
}
