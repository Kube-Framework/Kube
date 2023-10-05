/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of GPU
 */

#include <gtest/gtest.h>

#include <Kube/GPU/Buffer.hpp>
#include <Kube/GPU/MemoryAllocation.hpp>

#include "GPUTestUtils.hpp"

using namespace kF;

TEST(MemoryAllocation, Basics)
{
    Tests::GPUInstance instance {};
    auto buffer = GPU::Buffer::MakeStaging(1024);
    auto buffer2 = GPU::Buffer::MakeStaging(512);

    auto alloc = GPU::MemoryAllocation::MakeStaging(buffer);
    ASSERT_TRUE(alloc);
    auto moved = std::move(alloc);
    ASSERT_TRUE(moved);
    ASSERT_FALSE(alloc);
    alloc = GPU::MemoryAllocation::MakeStaging(buffer2);
    ASSERT_TRUE(alloc);
}