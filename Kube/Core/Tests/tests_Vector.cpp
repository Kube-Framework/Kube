/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector unit tests
 */

#include <gtest/gtest.h>

#include <string>

#define DECLARE_VECTOR(VariableName, AllocatorMode, ...)  DECLARE_VECTOR_##AllocatorMode(VariableName, __VA_ARGS__)

#define DECLARE_VECTOR_NO_ALLOCATOR(VariableName, ...)    VariableName __VA_OPT__((__VA_ARGS__))
#define DECLARE_VECTOR_ALLOCATOR(VariableName, ...)       VariableName(DefaultAllocator __VA_OPT__(,) __VA_ARGS__)

#define GENERATE_COMMON_VECTOR_TESTS(Vector, AllocatorMode, ...) \
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
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode);  \
 \
    ASSERT_FALSE(vector); \
    for (auto i = 0u; i < count; ++i) { \
        ASSERT_EQ(vector.push(i), i); \
        ASSERT_EQ(vector.size(), i + 1); \
    } \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    auto i = 0u; \
    for (const auto elem : vector) { \
        ASSERT_EQ(elem, i); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    vector.clear(); \
    ASSERT_FALSE(vector); \
    for (auto i = 0u; i < count; ++i) \
        vector.push(i); \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    i = 0u; \
    for (const auto elem : vector) { \
        ASSERT_EQ(elem, i); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
} \
 \
TEST(Vector, Pop) \
{ \
    constexpr auto count = 42u; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, count, 0u); \
 \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    for (auto i = 0u; i < count; ++i) { \
        ASSERT_EQ(vector.size(), count - i); \
        vector.pop(); \
    } \
    ASSERT_EQ(vector.size(), 0); \
} \
 \
TEST(Vector, NullResize) \
{ \
    Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, 0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
    vector.resize(0, 0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
} \
 \
TEST(Vector, Resize) \
{ \
    constexpr auto str = "Vector is an amazing 8 bytes vector !"; \
    constexpr auto str2 = "Hello"; \
    constexpr auto count = 4u; \
    constexpr auto count2 = 420u; \
 \
    Vector<std::string __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, count, str); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    auto i = 0u; \
    for (const auto &elem : vector) { \
        ASSERT_EQ(elem, str); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    vector.resize(count, str2); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    i = 0u; \
    for (const auto &elem : vector) { \
        ASSERT_EQ(elem, str2); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    std::vector<std::string> tmp(count * 2, str); \
    vector.resize(std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end())); \
    ASSERT_EQ(vector.size(), count * 2); \
    for (auto &elem : tmp) \
        ASSERT_TRUE(elem.empty()); \
    for (auto &elem : vector) \
        ASSERT_EQ(elem, str); \
    vector.resize(count2, [](const std::size_t i) { return std::to_string(i); }); \
    ASSERT_EQ(vector.size(), count2); \
    for (decltype(vector)::RangeType i = 0; i != count2; ++i) \
        ASSERT_EQ(vector[i], std::to_string(i)); \
    vector.resize(count2, [i = static_cast<std::size_t>(0)](void) mutable { return std::to_string(++i); }); \
    ASSERT_EQ(vector.size(), count2); \
    for (decltype(vector)::RangeType i = 0; i != count2; ++i) \
        ASSERT_EQ(vector[i], std::to_string(i + 1)); \
} \
 \
TEST(Vector, Reserve) \
{ \
    constexpr auto str = "Vector is an amazing 8 bytes vector !"; \
    constexpr auto count = 4u; \
 \
    Vector<std::string __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode);  \
 \
    vector.reserve(count); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), count); \
    vector.resize(count, str); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    vector.reserve(count - 1); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
} \
 \
TEST(Vector, Move) \
{ \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode);  \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Original, AllocatorMode, std::initializer_list<std::size_t>({ 0, 1, 2, 3, 4 })); \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Answer1, AllocatorMode,  std::initializer_list<std::size_t>({ 3, 0, 1, 2, 4 })); \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Answer2, AllocatorMode,  std::initializer_list<std::size_t>({ 3, 4, 0, 1, 2 })); \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Answer3, AllocatorMode,  std::initializer_list<std::size_t>({ 1, 2, 3, 4, 0 })); \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Answer4, AllocatorMode,  std::initializer_list<std::size_t>({ 2, 3, 4, 0, 1 })); \
    const Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(Answer5, AllocatorMode, std::initializer_list<std::size_t>({ 1, 0, 2, 3, 4 })); \
    vector = Original; \
    ASSERT_EQ(vector, Original); \
    vector.move(3, 3, 0); \
    ASSERT_EQ(vector, Original); \
    vector.move(3, 4, 0); \
    ASSERT_EQ(vector, Answer1); \
    vector = Original; \
    vector.move(3, 5, 0); \
    ASSERT_EQ(vector, Answer2); \
    vector = Original; \
    vector.move(0, 1, 4); \
    ASSERT_EQ(vector, Answer3); \
    vector = Original; \
    vector.move(0, 2, 4); \
    ASSERT_EQ(vector, Answer4); \
    vector = Original; \
    vector.move(0, 1, 1); \
    ASSERT_EQ(vector, Answer5); \
} \
 \
TEST(Vector, InsertIterators) \
{ \
    std::vector<int> tmp(10, 42); \
    std::vector<int> tmp2(5, 32); \
    Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, tmp.begin(), tmp.end()); \
 \
    ASSERT_EQ(vector.size(), 10u); \
    for (auto elem : vector) \
        ASSERT_EQ(elem, 42); \
    vector.insert(vector.begin() + 1, tmp2.begin(), tmp2.end()); \
    ASSERT_EQ(vector[0], 42); \
    for (auto i = 0u; i < 5u; ++i) \
        ASSERT_EQ(vector[1 + i], 32); \
    for (auto i = 0u; i < 9; ++i) \
        ASSERT_EQ(vector[6 + i], 42); \
    vector.insert(vector.end(), { 45, 46 }); \
    ASSERT_EQ(vector.at(vector.size() - 2), 45); \
    ASSERT_EQ(vector.back(), 46); \
} \
TEST(Vector, InsertMap) \
{ \
    std::vector<std::string> tmp(10, std::string("42")); \
    std::vector<std::string> tmp2(5, std::string("32")); \
    Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, tmp.begin(), tmp.end(), [](const std::string &str) { \
        return std::stoi(str); \
    }); \
 \
    ASSERT_EQ(vector.size(), 10u); \
    for (auto elem : vector) \
        ASSERT_EQ(elem, 42); \
    vector.insert(vector.begin() + 1, tmp2.begin(), tmp2.end(), [](const std::string &str) { \
        return std::stoi(str); \
    }); \
    ASSERT_EQ(vector[0], 42); \
    for (auto i = 0u; i < 5u; ++i) \
        ASSERT_EQ(vector[1 + i], 32); \
    for (auto i = 0u; i < 9; ++i) \
        ASSERT_EQ(vector[6 + i], 42); \
} \
 \
TEST(Vector, InsertFill) \
{ \
    Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode);  \
 \
    vector.insertFill(vector.begin(), 2, 42); \
    ASSERT_EQ(vector.size(), 2); \
    for (auto &elem : vector) \
        ASSERT_EQ(elem, 42); \
    vector.insertFill(vector.begin(), 1, 32); \
    ASSERT_EQ(vector.front(), 32); \
    vector.insertFill(vector.end(), 1, 32); \
    ASSERT_EQ(vector.back(), 32); \
    vector.insertFill(vector.end(), 42, 32); \
    for (auto i = 4u; i < vector.size(); ++i) \
        ASSERT_EQ(vector[i], 32); \
} \
 \
TEST(Vector, Clear) \
{ \
    constexpr auto count = 42; \
    constexpr auto value1 = 24; \
    constexpr auto value2 = 42; \
    Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, count, value1); \
    auto *data = vector.data(); \
    for (auto i = 0u; i < count; ++i) \
        ASSERT_EQ(data[i], value1); \
    vector.clear(); \
    vector.insertFill(vector.begin(), count, value2); \
    for (auto i = 0u; i < count; ++i) \
            ASSERT_EQ(data[i], value2); \
    vector.clear(); \
    vector.insertFill(vector.begin(), count * 2, value1); \
    ASSERT_NE(vector.data(), data); \
} \
 \
TEST(Vector, Erase) \
{ \
    constexpr auto count = 10u; \
 \
    constexpr auto Get = [](const auto count) { \
        Vector<int __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode, count); \
        for (auto i = 0u; i < count; ++i) \
            vector[i] = i; \
        return vector; \
    }; \
 \
    { \
        auto vector = Get(count); \
        vector.erase(vector.begin(), vector.end()); \
        ASSERT_EQ(vector.size(), 0); \
    } \
    { \
        auto vector = Get(count); \
        vector.erase(vector.end() - count / 2, vector.end()); \
        ASSERT_EQ(vector.size(), count / 2); \
        auto i = 0u; \
        for (auto elem : vector) { \
            ASSERT_EQ(elem, i++); \
        } \
        vector.erase(vector.begin(), vector.size() - 1); \
        ASSERT_EQ(vector.size(), 1); \
        ASSERT_EQ(vector.front(), count / 2 - 1); \
        vector.erase(vector.begin()); \
        ASSERT_EQ(vector.size(), 0); \
    } \
} \
 \
TEST(Vector, Find) \
{ \
    constexpr auto count = 42u; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> DECLARE_VECTOR(vector, AllocatorMode);  \
 \
    ASSERT_FALSE(vector); \
    for (auto i = 0u; i < count; ++i) { \
        vector.push(i); \
    } \
    ASSERT_EQ(vector.find(0), vector.begin()); \
    ASSERT_EQ(vector.find(41), vector.end() - 1); \
    ASSERT_EQ(vector.find(42), vector.end()); \
    ASSERT_EQ(vector.find([](std::size_t x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](const std::size_t x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](const std::size_t &x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](std::size_t &x) { return ++x == 42; }), vector.end() - 1); \
}

#define GENERATE_COMMON_SMALL_VECTOR_TESTS(Vector, AllocatorMode, ...) \
TEST(Vector, SmallOptimizationInsertRange) \
{ \
    std::unique_ptr<int> data[] = { \
        std::make_unique<int>(0), std::make_unique<int>(1), \
        std::make_unique<int>(2), std::make_unique<int>(3) \
    }; \
    Vector<std::unique_ptr<int>, 4> DECLARE_VECTOR(vector, AllocatorMode, \
        std::make_move_iterator(std::begin(data)), \
        std::make_move_iterator(std::end(data)) \
    ); \
 \
    ASSERT_TRUE(vector.isCacheUsed()); \
    for (auto i = 0; i < 4; ++i) \
        ASSERT_EQ(*vector.at(i), i); \
 \
    vector.push(std::make_unique<int>(4)); \
    ASSERT_FALSE(vector.isCacheUsed()); \
    for (auto i = 0; i < 5; ++i) \
        ASSERT_EQ(*vector.at(i), i); \
} \
 \
TEST(Vector, SmallOptimizationPush) \
{ \
    constexpr auto PushTest = [](auto &vector, const int value, const bool isCacheUsed) { \
        vector.push(std::make_unique<int>(value)); \
        ASSERT_EQ(vector.isCacheUsed(), isCacheUsed); \
        for (auto i = 0; i <= value; ++i) \
            ASSERT_EQ(*vector.at(i), i); \
    }; \
    Vector<std::unique_ptr<int>, 4> DECLARE_VECTOR(vector, AllocatorMode); \
 \
    PushTest(vector, 0, true); \
    PushTest(vector, 1, true); \
    PushTest(vector, 2, true); \
    PushTest(vector, 3, true); \
 \
    PushTest(vector, 4, false); \
}


/** @brief Generate common vector tests */
#define GENERATE_VECTOR_TESTS(Vector, ...) \
    GENERATE_COMMON_VECTOR_TESTS(Vector, NO_ALLOCATOR __VA_OPT__(,) __VA_ARGS__)

/** @brief Generate common + allocated vector tests */
#define GENERATE_ALLOCATED_VECTOR_TESTS(Vector, ...) \
    GENERATE_COMMON_VECTOR_TESTS(Vector, ALLOCATOR __VA_OPT__(,) __VA_ARGS__)

/** @brief Generate common + small vector tests */
#define GENERATE_SMALL_VECTOR_TESTS(Vector, ...) \
    GENERATE_COMMON_VECTOR_TESTS(Vector, NO_ALLOCATOR, 4 __VA_OPT__(,) __VA_ARGS__) \
    GENERATE_COMMON_SMALL_VECTOR_TESTS(Vector, NO_ALLOCATOR __VA_OPT__(,) __VA_ARGS__)

/** @brief Generate common + allocated + small vector tests */
#define GENERATE_ALLOCATED_SMALL_VECTOR_TESTS(Vector, ...) \
    GENERATE_COMMON_VECTOR_TESTS(Vector, ALLOCATOR, 4 __VA_OPT__(,) __VA_ARGS__) \
    GENERATE_COMMON_SMALL_VECTOR_TESTS(Vector, ALLOCATOR __VA_OPT__(,) __VA_ARGS__)



#include <Kube/Core/Vector.hpp>
#include <Kube/Core/FlatVector.hpp>
#include <Kube/Core/SmallVector.hpp>
#include <Kube/Core/AllocatedVector.hpp>
#include <Kube/Core/AllocatedFlatVector.hpp>
#include <Kube/Core/AllocatedSmallVector.hpp>

using namespace kF::Core;

class DummyAllocator : public IAllocator
{
public:
    virtual ~DummyAllocator(void) override = default;

    [[nodiscard]] virtual void *allocate(const std::size_t size, const std::size_t alignment) noexcept
        { return AlignedAlloc(size, alignment); }

    virtual void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept
        { return AlignedFree(data, size, alignment); }
};

#include <Kube/Core/UnsafeAllocator.hpp>
// #include <Kube/Core/DebugAllocator.hpp>
// static DebugAllocator DefaultAllocator;
// static DummyAllocator<> DefaultAllocator;
static UnsafeAllocator<> DefaultAllocator;
static UnsafeAllocator<> DefaultAllocator2;


GENERATE_VECTOR_TESTS(Vector)
GENERATE_VECTOR_TESTS(LongVector)

GENERATE_VECTOR_TESTS(FlatVector)
GENERATE_VECTOR_TESTS(LongFlatVector)

GENERATE_SMALL_VECTOR_TESTS(SmallVector)
GENERATE_SMALL_VECTOR_TESTS(LongSmallVector)

GENERATE_ALLOCATED_VECTOR_TESTS(AllocatedVector)
GENERATE_ALLOCATED_VECTOR_TESTS(AllocatedLongVector)

GENERATE_ALLOCATED_VECTOR_TESTS(AllocatedFlatVector)
GENERATE_ALLOCATED_VECTOR_TESTS(AllocatedLongFlatVector)

GENERATE_ALLOCATED_SMALL_VECTOR_TESTS(AllocatedSmallVector)
GENERATE_ALLOCATED_SMALL_VECTOR_TESTS(AllocatedLongSmallVector)
