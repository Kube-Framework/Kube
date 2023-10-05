/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Kube's literals
 */

#include <gtest/gtest.h>

#include <Kube/Core/Expected.hpp>

using namespace kF;

enum class Code
{
    CodeA,
    CodeB,
    CodeC
};

TEST(Expected, TrivialAmbigous)
{
    Core::Expected<int, float> success(42);
    ASSERT_TRUE(success.success());
    ASSERT_EQ(success.value(), 42);

    Core::Expected<int, float> error(42.0f);
    ASSERT_FALSE(error.success());
    ASSERT_EQ(error.error(), 42.0f);
}

TEST(Expected, TrivialVoid)
{
    Core::Expected<int> success(42);
    ASSERT_TRUE(success.success());
    ASSERT_EQ(success.value(), 42);

    Core::Expected<int> error;
    ASSERT_FALSE(error.success());
}

TEST(Expected, TrivialEnum)
{
    Core::Expected<int, Code> success(42);
    ASSERT_TRUE(success.success());
    ASSERT_EQ(success.value(), 42);

    Core::Expected<int, Code> error(Code::CodeC);
    ASSERT_FALSE(error.success());
    ASSERT_EQ(error.error(), Code::CodeC);
}

TEST(Expected, NonCopyable)
{
    Core::Expected<std::unique_ptr<int>> success(std::make_unique<int>(42));
    ASSERT_TRUE(success.success());
    ASSERT_EQ(*success.value(), 42);

    Core::Expected<std::unique_ptr<int>> moved(std::move(success));
    ASSERT_TRUE(moved.success());
    ASSERT_EQ(*moved.value(), 42);

    Core::Expected<std::unique_ptr<int>> error;
    ASSERT_FALSE(error.success());
    error = std::move(moved);
    ASSERT_TRUE(error.success());
    ASSERT_EQ(*error.value(), 42);
}

