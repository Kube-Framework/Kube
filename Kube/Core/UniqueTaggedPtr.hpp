/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unique pointer using allocator
 */

#pragma once

#include "TaggedPtr.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t Alignment, kF::Core::StaticAllocatorRequirements Allocator>
    class UniqueTaggedPtr;
}

/** @brief Unique pointer class */
template<typename Type, std::size_t Alignment = alignof(Type), kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class kF::Core::UniqueTaggedPtr : public TaggedPtr<Type, Alignment>
{
public:
    /** @brief Allocate an instance */
    template<typename ...Args>
    [[nodiscard]] static inline UniqueTaggedPtr Make(Args &&...args) noexcept
        { return UniqueTaggedPtr(new (Allocator::Allocate(sizeof(Type), alignof(Type))) Type(std::forward<Args>(args)...)); }


    /** @brief Destructor */
    inline ~UniqueTaggedPtr(void) noexcept { release(); }

    /** @brief Construct the unique pointer */
    inline UniqueTaggedPtr(void) noexcept = default;

    /** @brief Move constructor */
    inline UniqueTaggedPtr(UniqueTaggedPtr &&other) noexcept { swap(other); }

    /** @brief Move assignment */
    inline UniqueTaggedPtr &operator=(UniqueTaggedPtr &&other) noexcept { swap(other); return *this; }


    /** @brief Swaps the managed objects */
    inline void swap(UniqueTaggedPtr &other) noexcept { swap(other); }

    /** @brief Destroy the managed object */
    inline void release(void) noexcept
    { if (const auto data = this->get(); data) { data->~Type(); Allocator::Deallocate(data, sizeof(Type), alignof(Type)); } }

private:
    using TaggedPtr<Type, Alignment>::set;
    using TaggedPtr<Type, Alignment>::reset;

    /** @brief Instance constructor */
    inline UniqueTaggedPtr(Type * const ptr) noexcept : TaggedPtr<Type, Alignment>(ptr) {}
};