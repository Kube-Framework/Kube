/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedFlatVectorBase
 */

#pragma once

#include "FlatVectorBase.hpp"
#include "IAllocator.hpp"

namespace kF::Core::Internal
{
    template<typename Type, typename CustomHeaderType, std::integral Range>
    class AllocatedFlatVectorBase;
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, typename CustomHeaderType, std::integral Range>
class kF::Core::Internal::AllocatedFlatVectorBase
        : public FlatVectorBase<Type, DefaultStaticAllocator, CustomHeaderType, Range>
{
public:
    /** @brief Base class */
    using Base = FlatVectorBase<Type, DefaultStaticAllocator, CustomHeaderType, Range>;

    /** @brief Base class header */
    using Header = Base::Header;


    /** @brief Default constructor, unsafe state */
    AllocatedFlatVectorBase(void) noexcept : _allocator(nullptr) {}

    /** @brief Every allocated vector need the reference over their allocator at construction */
    AllocatedFlatVectorBase(IAllocator &allocator) noexcept : _allocator(&allocator) {}


    /** @brief Steal another instance */
    void steal(AllocatedFlatVectorBase &other) noexcept { Base::steal(other); _allocator = other._allocator; }

    /** @brief Swap two instances */
    void swap(AllocatedFlatVectorBase &other) noexcept { Base::swap(other); std::swap(_allocator, other._allocator); }


    /** @brief Get internal allocator */
    [[nodiscard]] IAllocator &allocator(void) const noexcept { return *_allocator; }

protected:
    /** @brief Set internal allocator */
    void setAllocator(IAllocator *allocator) noexcept { _allocator = allocator; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept;

private:
    IAllocator *_allocator {};

    using Base::_ptr;
};

#include "AllocatedFlatVectorBase.ipp"