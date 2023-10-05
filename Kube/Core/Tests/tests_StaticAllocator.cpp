/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log tests
 */

#include <string>
#include <sstream>

#include <gtest/gtest.h>

#include <Kube/Core/StaticAllocator.hpp>
#include <Kube/Core/UnsafeAllocator.hpp>

using namespace kF;
using namespace kF::Core::Literal;

TEST(StaticAllocator, StaticUnsafeAllocator)
{
    using StaticUnsafeAllocator = Core::StaticAllocator<Core::UnsafeAllocator<>, "TestStaticUnsafeAllocator">;

    auto ptr = StaticUnsafeAllocator::Allocate(42, 16);
    ASSERT_NE(ptr, nullptr);
    StaticUnsafeAllocator::Deallocate(ptr, 64, 16);
}