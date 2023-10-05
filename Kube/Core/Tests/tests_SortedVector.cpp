/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted Vector unit tests
 */

#include <gtest/gtest.h>

#include <string>

#define DECLARE_VECTOR(VariableName, AllocatorMode, ...)  DECLARE_VECTOR_##AllocatorMode(VariableName, __VA_ARGS__)

#define DECLARE_VECTOR_NO_ALLOCATOR(VariableName, ...)    VariableName __VA_OPT__((__VA_ARGS__))
#define DECLARE_VECTOR_ALLOCATOR(VariableName, ...)       VariableName(DefaultAllocator __VA_OPT__(,) __VA_ARGS__)

#define GENERATE_VECTOR_TESTS(Vector, AllocatorMode, ...) \
TEST(Vector, Basics) \
{ \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, 0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
} \
 \
TEST(Vector, Semantics) \
{ \
    for (auto i = 1; i < 15; ++i) { \
        Vector<std::string __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, i, "Hello World 123456789"); \
        auto copy1(vector); \
        auto copy2 = copy1; \
        auto tmp1 = copy2; \
        auto tmp2 = tmp1; \
        auto move1(std::move(tmp1)); \
        auto move2 = std::move(tmp2); \
        ASSERT_EQ(vector, copy1); \
        ASSERT_EQ(vector, copy2); \
        ASSERT_EQ(vector, move1); \
        ASSERT_EQ(vector, move2); \
        ASSERT_NE(vector, tmp1); \
        ASSERT_NE(vector, tmp2); \
    } \
} \
 \
TEST(Vector, Push) \
{ \
    constexpr auto count = 42u; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode); \
 \
    ASSERT_FALSE(vector); \
    for (auto i = 0u, j = count; i < count; ++i, --j) { \
        ASSERT_EQ(vector.push(j), j); \
        ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    } \
    ASSERT_EQ(vector.size(), count); \
    std::size_t i = 1ul; \
    for (auto &elem : vector) { \
        ASSERT_EQ(elem, i); \
        ++i; \
    } \
} \
 \
TEST(Vector, Insert) \
{ \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode); \
    const std::string tmp[] { "42", "42" }; \
 \
    vector.insertDefault(2u); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 2); \
    auto i = 0u; \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 0u); \
 \
    vector.insert(std::begin(tmp), std::end(tmp), [](const auto &x) { return std::stoul(x); }); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 4); \
    i = 0u; \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 42u); \
    ASSERT_EQ(vector[i++], 42u); \
 \
    vector.insertFill(2u, 24u); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 6); \
    i = 0u; \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 24u); \
    ASSERT_EQ(vector[i++], 24u); \
    ASSERT_EQ(vector[i++], 42u); \
    ASSERT_EQ(vector[i++], 42u); \
 \
    vector.insert(vector.end() - 2, vector.end()); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 8); \
    i = 0u; \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 0u); \
    ASSERT_EQ(vector[i++], 24u); \
    ASSERT_EQ(vector[i++], 24u); \
    ASSERT_EQ(vector[i++], 42u); \
    ASSERT_EQ(vector[i++], 42u); \
    ASSERT_EQ(vector[i++], 42u); \
    ASSERT_EQ(vector[i++], 42u); \
} \
 \
TEST(Vector, Resize) \
{ \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode); \
 \
    vector.resize(4); \
    ASSERT_EQ(vector.size(), 4); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    for (auto &elem : vector) { \
        ASSERT_EQ(elem, 0ul); \
    } \
 \
    const std::string tmp[] { "42", "1", "2", "3", "4" }; \
    vector.resize(std::begin(tmp), std::end(tmp), [](const auto &x) { return std::stoul(x); }); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 5); \
    ASSERT_EQ(vector[0], 1); \
    ASSERT_EQ(vector[1], 2); \
    ASSERT_EQ(vector[2], 3); \
    ASSERT_EQ(vector[3], 4); \
    ASSERT_EQ(vector[4], 42); \
 \
    vector.resize(8ul, 24ul); \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 8); \
    for (auto &elem : vector) { \
        ASSERT_EQ(elem, 24ul); \
    } \
\
    std::size_t tmp2[] = { 8, 9, 5, 4, 6, 3, 2 ,1, 7, 0 }; \
    vector.resize(std::begin(tmp2), std::end(tmp2)); \
    std::size_t i = 0u; \
    ASSERT_TRUE(std::is_sorted(vector.begin(), vector.end())); \
    ASSERT_EQ(vector.size(), 10); \
    for (auto &elem : vector) { \
        ASSERT_EQ(elem, i++); \
    } \
    ASSERT_EQ(i, 10); \
} \
TEST(Vector, NullArgs) \
{ \
    std::size_t *ptr = nullptr; \
    std::string *strPtr = nullptr; \
 \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(a, AllocatorMode, 0u); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(b, AllocatorMode, 0u, 42u); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(c, AllocatorMode, ptr, ptr); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(d, AllocatorMode, strPtr, strPtr, [](auto &x) { return std::stoul(x); }); \
    ASSERT_EQ(a.size(), 0ul); ASSERT_TRUE(a.empty()); \
    ASSERT_EQ(b.size(), 0ul); ASSERT_TRUE(b.empty()); \
    ASSERT_EQ(c.size(), 0ul); ASSERT_TRUE(c.empty()); \
    ASSERT_EQ(d.size(), 0ul); ASSERT_TRUE(d.empty()); \
 \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(e, AllocatorMode); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(f, AllocatorMode); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(g, AllocatorMode); \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(h, AllocatorMode); \
    e.insertDefault(0u); \
    f.insertFill(0u, 42u); \
    g.insert(ptr, ptr); \
    h.insert(strPtr, strPtr, [](auto &x) { return std::stoul(x); }); \
    ASSERT_EQ(e.size(), 0ul); ASSERT_TRUE(e.empty()); \
    ASSERT_EQ(f.size(), 0ul); ASSERT_TRUE(f.empty()); \
    ASSERT_EQ(g.size(), 0ul); ASSERT_TRUE(g.empty()); \
    ASSERT_EQ(h.size(), 0ul); ASSERT_TRUE(h.empty()); \
}

#include <Kube/Core/SortedVector.hpp>
#include <Kube/Core/SortedFlatVector.hpp>
#include <Kube/Core/SortedSmallVector.hpp>
#include <Kube/Core/SortedAllocatedVector.hpp>
#include <Kube/Core/SortedAllocatedFlatVector.hpp>
#include <Kube/Core/SortedAllocatedSmallVector.hpp>

using namespace kF::Core;

#include <Kube/Core/UnsafeAllocator.hpp>
static UnsafeAllocator<> DefaultAllocator;


GENERATE_VECTOR_TESTS(SortedVector, NO_ALLOCATOR)
GENERATE_VECTOR_TESTS(SortedLongVector, NO_ALLOCATOR)

GENERATE_VECTOR_TESTS(SortedFlatVector, NO_ALLOCATOR)
GENERATE_VECTOR_TESTS(SortedLongFlatVector, NO_ALLOCATOR)

GENERATE_VECTOR_TESTS(SortedSmallVector, NO_ALLOCATOR, 4)
GENERATE_VECTOR_TESTS(SortedLongSmallVector, NO_ALLOCATOR, 4)

GENERATE_VECTOR_TESTS(SortedAllocatedVector, ALLOCATOR)
GENERATE_VECTOR_TESTS(SortedAllocatedLongVector, ALLOCATOR)

GENERATE_VECTOR_TESTS(SortedAllocatedFlatVector, ALLOCATOR)
GENERATE_VECTOR_TESTS(SortedAllocatedLongFlatVector, ALLOCATOR)

GENERATE_VECTOR_TESTS(SortedAllocatedSmallVector, ALLOCATOR, 4)
GENERATE_VECTOR_TESTS(SortedAllocatedLongSmallVector, ALLOCATOR, 4)