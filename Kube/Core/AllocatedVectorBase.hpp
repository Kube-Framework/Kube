/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedVectorBase
 */

#pragma once

#include "VectorBase.hpp"
#include "IAllocator.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::integral Range>
    class AllocatedVectorBase;
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, std::integral Range>
class kF::Core::Internal::AllocatedVectorBase
        : public VectorBase<Type, DefaultStaticAllocator, Range>
{
public:
    /** @brief Base class */
    using Base = VectorBase<Type, DefaultStaticAllocator, Range>;


    /** @brief Default constructor, unsafe state */
    inline AllocatedVectorBase(void) noexcept = default;

    /** @brief Every allocated vector need the reference over their allocator at construction */
    inline AllocatedVectorBase(IAllocator &allocator) noexcept : _allocator(&allocator) {}


    /** @brief Steal another instance */
    inline void steal(AllocatedVectorBase &other) noexcept { Base::steal(other); _allocator = other._allocator; }

    /** @brief Swap two instances */
    inline void swap(AllocatedVectorBase &other) noexcept { Base::swap(other); std::swap(_allocator, other._allocator); }


    /** @brief Get internal allocator */
    [[nodiscard]] inline IAllocator &allocator(void) const noexcept { return *_allocator; }

protected:
    /** @brief Set internal allocator */
    inline void setAllocator(IAllocator *allocator) noexcept { _allocator = allocator; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] inline Type *allocate(const Range capacity) noexcept
        { return reinterpret_cast<Type *>(_allocator->allocate(sizeof(Type) * capacity, alignof(Type))); }

    /** @brief Deallocates a buffer */
    inline void deallocate(Type * const data, const Range capacity) noexcept
        { _allocator->deallocate(data, sizeof(Type) * capacity, alignof(Type)); }

private:
    IAllocator *_allocator {};
};
