/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of File
 */

#include <gtest/gtest.h>

#include <Kube/IO/ResourceManager.hpp>
#include <Kube/IO/File.hpp>

using namespace kF;

KF_DECLARE_RESOURCE_ENVIRONMENT(IOTests);

constexpr std::string_view EnvironmentName = "IOTests";
constexpr Core::HashedName EnvironmentHash = Core::Hash(EnvironmentName);

constexpr std::string_view Test01Path = ":/IOTests/FileTest01.txt";
constexpr std::string_view Test01ResourcePath = "FileTest01.txt";
constexpr std::string_view Test01ContentText = "Kube Framework !";

constexpr std::string_view WrongPath = ":/IOTests/FileTestZZ.txt";
constexpr std::string_view WrongResourcePath = "FileTestZZ.txt";

TEST(File, Test01)
{
    IO::ResourceManager manager;
    IO::File file(Test01Path);

    // Infos
    ASSERT_TRUE(file.isResource());
    ASSERT_EQ(file.path(), Test01Path);
    ASSERT_EQ(file.environment(), EnvironmentName);
    ASSERT_EQ(file.environmentHash(), EnvironmentHash);
    ASSERT_EQ(file.resourcePath(), Test01ResourcePath);

    // Exists
    ASSERT_TRUE(file.resourceExists());

    // Query
    const auto range = file.queryResource();
    std::string_view view(reinterpret_cast<const char *>(range.begin()), range.size());
    ASSERT_EQ(view, Test01ContentText);
}

TEST(File, NonExisting)
{

    IO::ResourceManager manager;
    IO::File file(WrongPath);

    // Infos
    ASSERT_TRUE(file.isResource());
    ASSERT_EQ(file.path(), WrongPath);
    ASSERT_EQ(file.environment(), EnvironmentName);
    ASSERT_EQ(file.environmentHash(), EnvironmentHash);
    ASSERT_EQ(file.resourcePath(), WrongResourcePath);

    // Query must fail
    ASSERT_FALSE(file.resourceExists());
    const auto range = file.queryResource();
    ASSERT_EQ(range.begin(), nullptr);
    ASSERT_EQ(range.end(), nullptr);
}

TEST(File, Dangerous)
{
    IO::File file(WrongPath);

    // Infos
    ASSERT_TRUE(file.isResource());
    ASSERT_EQ(file.path(), WrongPath);
    ASSERT_EQ(file.environment(), EnvironmentName);
    ASSERT_EQ(file.environmentHash(), EnvironmentHash);
    ASSERT_EQ(file.resourcePath(), WrongResourcePath);

    // Queries must crash when resource manager is not initialized
    ASSERT_DEATH(
        [file] { ASSERT_FALSE(file.resourceExists()); }(), ""
    );
    ASSERT_DEATH(
        [file] { ASSERT_EQ(file.queryResource().begin(), nullptr); }(), ""
    );
}