/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log tests
 */

#include <string>
#include <sstream>

#include <gtest/gtest.h>

#include <Kube/Core/Log.hpp>

#include <codecvt>

using namespace kF;

TEST(Log, Singletons)
{
    constexpr auto Answer = "Hello world";

    std::string output;

    // Info
    testing::internal::CaptureStdout();
    kFInfoRaw(Answer);
    output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, Answer);

    // Error
    testing::internal::CaptureStderr();
    kFErrorRaw(Answer);
    output = testing::internal::GetCapturedStderr();
    ASSERT_EQ(output, Answer);
}

TEST(Log, Basics)
{
    std::ostringstream oss;
    Core::Log log(oss);
    log.log("The key is ", 42);
    ASSERT_EQ(oss.str(), "The key is 42\n");
}