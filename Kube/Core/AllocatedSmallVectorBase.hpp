/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocated Small Vector
 */

#pragma once

#include "SmallVectorBase.hpp"
#include "IAllocator.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
    class AllocatedSmallVectorBase;

    struct IntrusiveSmallVectorRuntimeAllocator
    {
    protected:
        IAllocator *_allocator {};
    };
}

/** @brief Base implementation of a vector with size and capacity cached */
template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
class kF::Core::Internal::AllocatedSmallVectorBase
        : public IntrusiveSmallVectorBase<Type, OptimizedCapacity, DefaultStaticAllocator, Range, IntrusiveSmallVectorRuntimeAllocator>
{
public:
    /** @brief Base class */
    using Base = IntrusiveSmallVectorBase<Type, OptimizedCapacity, DefaultStaticAllocator, Range, IntrusiveSmallVectorRuntimeAllocator>;

    /** @brief Output iterator */
    using Iterator = Type *;

    /** @brief Input iterator */
    using ConstIterator = const Type *;

    /** @brief Used base functions */
    using Base::optimizedData;


    /** @brief Default constructor, unsafe state */
    inline AllocatedSmallVectorBase(void) noexcept = default;

    /** @brief Every allocated vector need the reference over their allocator at construction */
    inline AllocatedSmallVectorBase(IAllocator &allocator) noexcept { _allocator = &allocator; }


    /** @brief Steal another instance */
    inline void steal(AllocatedSmallVectorBase &other) noexcept
        { Base::steal(other); _allocator = other._allocator; }

    /** @brief Swap two instances */
    inline void swap(AllocatedSmallVectorBase &other) noexcept
        { Base::swap(other); std::swap(_allocator, other._allocator); }


    /** @brief Get internal allocator */
    [[nodiscard]] inline IAllocator &allocator(void) const noexcept { return *_allocator; }

protected:
    /** @brief Set internal allocator */
    inline void setAllocator(IAllocator *allocator) noexcept { _allocator = allocator; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept;

private:
    using Base::_allocator;
};

#include "AllocatedSmallVectorBase.ipp"