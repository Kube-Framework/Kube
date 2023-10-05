/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of StandardPaths
 */

#include <filesystem>

#include <gtest/gtest.h>

#include <Kube/Core/String.hpp>
#include <Kube/IO/StandardPaths.hpp>


using namespace kF;

TEST(StandardPaths, GetExecutablePath)
{
    const auto str = IO::GetExecutablePath<Core::String<>>();

    ASSERT_GT(str.size(), 0);
    ASSERT_TRUE(std::filesystem::exists(str.toView()));
}


TEST(StandardPaths, GetExecutableDirectory)
{
    const auto str = IO::GetExecutableDirectory<Core::String<>>();

    ASSERT_GT(str.size(), 0);
    ASSERT_TRUE(std::filesystem::exists(str.toView()));
}
