/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/TaggedPtr.hpp>

using namespace kF;

template<std::size_t Alignment, typename Type, typename TagType>
static void TestTaggedPtr(const Core::TaggedPtr<Type, Alignment> &tagged, Type * const ptr, const TagType tag) noexcept
{
    const bool isNull = ptr == nullptr;

    // Pointer and tag dereference
    ASSERT_EQ(tagged.operator bool(), !isNull);
    ASSERT_EQ(tagged.get(), ptr);
    ASSERT_EQ(tagged.tag(), tag);
    ASSERT_EQ(tagged.operator->(), ptr);
    ASSERT_EQ(&tagged.operator*(), ptr);

    // nullptr_t comparison
    if (isNull) {
        ASSERT_TRUE(nullptr == tagged);
        ASSERT_TRUE(tagged == nullptr);
        ASSERT_FALSE(nullptr != tagged);
        ASSERT_FALSE(tagged != nullptr);
    } else {
        ASSERT_TRUE(nullptr != tagged);
        ASSERT_TRUE(tagged != nullptr);
        ASSERT_FALSE(nullptr == tagged);
        ASSERT_FALSE(tagged == nullptr);
    }
}

TEST(TaggedPtr, Basics)
{
    constexpr auto Alignment = 16ul;
    auto ptr = reinterpret_cast<std::uint32_t *>(Core::AlignedAlloc(sizeof(std::uint32_t), Alignment));
    *ptr = 42;
    std::uint32_t tag = Alignment / 2;

    // Constructor(ptr, Max / 2)
    Core::TaggedPtr<std::uint32_t, Alignment> tagged(ptr, tag);
    TestTaggedPtr<Alignment, std::uint32_t, std::uint32_t>(tagged, ptr, tag);

    // setTag(Max)
    tag = Alignment - 1;
    tagged.setTag(tag);
    TestTaggedPtr<Alignment, std::uint32_t, std::uint32_t>(tagged, ptr, tag);

    // set(null)
    tagged.set(nullptr);
    TestTaggedPtr<Alignment, std::uint32_t, std::uint32_t>(tagged, nullptr, tag);

    // set(ptr, Max + 1)
    tag = Alignment;
    tagged.set(ptr, tag);
    TestTaggedPtr<Alignment, std::uint32_t, std::uint32_t>(tagged, ptr, 0u);

    Core::AlignedFree(tagged.get(), sizeof(std::uint32_t), Alignment);
}

TEST(TaggedPtr, AmbigousComparison)
{
    Core::TaggedPtr<void *, 2> p1(nullptr, 0);
    Core::TaggedPtr<void *, 2> p2(nullptr, 1);

    ASSERT_NE(p1, p2);
    p1.setTag(1);
    ASSERT_EQ(p1, p2);
}