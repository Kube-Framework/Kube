/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#define DECLARE_STRING(VariableName, AllocatorMode, ...)    DECLARE_STRING_##AllocatorMode(VariableName, __VA_ARGS__)
#define DECLARE_STRING_NO_ALLOCATOR(VariableName, ...)      VariableName __VA_OPT__((__VA_ARGS__))
#define DECLARE_STRING_ALLOCATOR(VariableName, ...)         VariableName(DefaultAllocator __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_STRING_RVALUE(AllocatorMode, ...)           DECLARE_STRING_RVALUE_##AllocatorMode(__VA_ARGS__)
#define DECLARE_STRING_RVALUE_NO_ALLOCATOR(...)             (__VA_ARGS__)
#define DECLARE_STRING_RVALUE_ALLOCATOR(...)                (DefaultAllocator __VA_OPT__(,) __VA_ARGS__)

#define GENERATE_STRING_TESTS(String, AllocatorMode, ...) \
using String##Class = String __VA_OPT__(<__VA_ARGS__>); \
TEST(String, Basics) \
{ \
    constexpr const char *value = "hello world"; \
    char array[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' , '\0' }; \
 \
    auto assertStringValue = [value, &array](const auto &str) { \
        ASSERT_EQ(str, value); \
        ASSERT_EQ(str, array); \
        ASSERT_EQ(str, std::string_view(value)); \
    }; \
 \
    String##Class DECLARE_STRING(str, AllocatorMode, value); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class DECLARE_STRING_RVALUE(AllocatorMode, array); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class DECLARE_STRING_RVALUE(AllocatorMode, std::string_view(value)); \
    assertStringValue(str); \
 \
    str = nullptr; \
    ASSERT_EQ(str, nullptr); \
    str = value; \
    assertStringValue(str); \
    str = array; \
    assertStringValue(str); \
    str = std::string_view(value); \
}

#define GENERATE_WSTRING_TESTS(String, AllocatorMode, ...) \
using String##Class = String __VA_OPT__(<__VA_ARGS__>); \
TEST(String, Basics) \
{ \
    constexpr const wchar_t *value = L"hello world"; \
    wchar_t array[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' , '\0' }; \
 \
    auto assertStringValue = [value, &array](const auto &str) { \
        ASSERT_EQ(str, value); \
        ASSERT_EQ(str, array); \
        ASSERT_EQ(str, std::wstring_view(value)); \
    }; \
 \
    String##Class DECLARE_STRING(str, AllocatorMode, value); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class DECLARE_STRING_RVALUE(AllocatorMode, array); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class DECLARE_STRING_RVALUE(AllocatorMode, std::wstring_view(value)); \
    assertStringValue(str); \
 \
    str = nullptr; \
    ASSERT_EQ(str, nullptr); \
    str = value; \
    assertStringValue(str); \
    str = array; \
    assertStringValue(str); \
    str = std::wstring_view(value); \
}

#include <Kube/Core/String.hpp>
#include <Kube/Core/FlatString.hpp>
#include <Kube/Core/SmallString.hpp>
#include <Kube/Core/AllocatedString.hpp>
#include <Kube/Core/AllocatedFlatString.hpp>
#include <Kube/Core/AllocatedSmallString.hpp>

static_assert_sizeof_quarter_cacheline(kF::Core::String<>);
static_assert_sizeof(kF::Core::LongString<>, 3 * kF::Core::CacheLineEighthSize);
static_assert_sizeof_half_cacheline(kF::Core::SmallString<>);
static_assert_sizeof_half_cacheline(kF::Core::LongSmallString<>);
static_assert_sizeof(kF::Core::FlatString<>, kF::Core::CacheLineEighthSize);
static_assert_sizeof(kF::Core::LongFlatString<>, kF::Core::CacheLineEighthSize);

static_assert_sizeof_quarter_cacheline(kF::Core::WString<>);
static_assert_sizeof(kF::Core::LongWString<>, 3 * kF::Core::CacheLineEighthSize);
static_assert_sizeof_cacheline(kF::Core::SmallWString<>);
static_assert_sizeof_cacheline(kF::Core::LongSmallWString<>);
static_assert_sizeof(kF::Core::FlatWString<>, kF::Core::CacheLineEighthSize);
static_assert_sizeof(kF::Core::LongFlatWString<>, kF::Core::CacheLineEighthSize);

using namespace kF::Core;

#include <Kube/Core/UnsafeAllocator.hpp>
static UnsafeAllocator<> DefaultAllocator;


GENERATE_STRING_TESTS(String, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_STRING_TESTS(LongString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_STRING_TESTS(FlatString, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_STRING_TESTS(LongFlatString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_STRING_TESTS(SmallString, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_STRING_TESTS(LongSmallString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_STRING_TESTS(AllocatedString, ALLOCATOR)
GENERATE_STRING_TESTS(AllocatedLongString, ALLOCATOR)

GENERATE_STRING_TESTS(AllocatedFlatString, ALLOCATOR)
GENERATE_STRING_TESTS(AllocatedLongFlatString, ALLOCATOR)

GENERATE_STRING_TESTS(AllocatedSmallString, ALLOCATOR)
GENERATE_STRING_TESTS(AllocatedLongSmallString, ALLOCATOR)

GENERATE_WSTRING_TESTS(WString, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_WSTRING_TESTS(LongWString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_WSTRING_TESTS(FlatWString, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_WSTRING_TESTS(LongFlatWString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_WSTRING_TESTS(SmallWString, NO_ALLOCATOR, DefaultStaticAllocator)
GENERATE_WSTRING_TESTS(LongSmallWString, NO_ALLOCATOR, DefaultStaticAllocator)

GENERATE_WSTRING_TESTS(AllocatedWString, ALLOCATOR)
GENERATE_WSTRING_TESTS(AllocatedLongWString, ALLOCATOR)

GENERATE_WSTRING_TESTS(AllocatedFlatWString, ALLOCATOR)
GENERATE_WSTRING_TESTS(AllocatedLongFlatWString, ALLOCATOR)

GENERATE_WSTRING_TESTS(AllocatedSmallWString, ALLOCATOR)
GENERATE_WSTRING_TESTS(AllocatedLongSmallWString, ALLOCATOR)
