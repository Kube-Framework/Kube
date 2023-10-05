/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorBase
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
    class VectorBase;
}

/** @brief Base implementation of a vector with size and capacity cached */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
class kF::Core::Internal::VectorBase
{
public:
    /** @brief Output iterator */
    using Iterator = Type *;

    /** @brief Input iterator */
    using ConstIterator = const Type *;


    /** @brief Always safe ! */
    [[nodiscard]] constexpr bool isSafe(void) const noexcept { return true; }

    /** @brief Fast empty check */
    [[nodiscard]] inline bool empty(void) const noexcept { return !_size; }


    /** @brief Get internal data pointer */
    [[nodiscard]] inline Type *data(void) noexcept { return dataUnsafe(); }
    [[nodiscard]] inline const Type *data(void) const noexcept { return dataUnsafe(); }
    [[nodiscard]] inline Type *dataUnsafe(void) noexcept { return _data; }
    [[nodiscard]] inline const Type *dataUnsafe(void) const noexcept { return _data; }


    /** @brief Get the size of the vector */
    [[nodiscard]] inline Range size(void) const noexcept { return sizeUnsafe(); }
    [[nodiscard]] inline Range sizeUnsafe(void) const noexcept { return _size; }


    /** @brief Get the capacity of the vector */
    [[nodiscard]] inline Range capacity(void) const noexcept { return capacityUnsafe(); }
    [[nodiscard]] inline Range capacityUnsafe(void) const noexcept { return _capacity; }


    /** @brief Begin / end overloads */
    [[nodiscard]] inline Iterator begin(void) noexcept { return beginUnsafe(); }
    [[nodiscard]] inline Iterator end(void) noexcept { return endUnsafe(); }
    [[nodiscard]] inline ConstIterator begin(void) const noexcept { return beginUnsafe(); }
    [[nodiscard]] inline ConstIterator end(void) const noexcept { return endUnsafe(); }

    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] inline Iterator beginUnsafe(void) noexcept { return dataUnsafe(); }
    [[nodiscard]] inline Iterator endUnsafe(void) noexcept { return dataUnsafe() + sizeUnsafe(); }
    [[nodiscard]] inline ConstIterator beginUnsafe(void) const noexcept { return dataUnsafe(); }
    [[nodiscard]] inline ConstIterator endUnsafe(void) const noexcept { return dataUnsafe() + sizeUnsafe(); }


    /** @brief Steal another instance */
    void steal(VectorBase &other) noexcept;

    /** @brief Swap two instances */
    void swap(VectorBase &other) noexcept;

protected:
    /** @brief Protected data setter */
    inline void setData(Type * const data) noexcept { _data = data; }

    /** @brief Protected size setter */
    inline void setSize(const Range size) noexcept { _size = size; }

    /** @brief Protected capacity setter */
    inline void setCapacity(const Range capacity) noexcept { _capacity = capacity; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] inline Type *allocate(const Range capacity) noexcept
        { return reinterpret_cast<Type *>(Allocator::Allocate(sizeof(Type) * static_cast<std::size_t>(capacity), alignof(Type))); }

    /** @brief Deallocates a buffer */
    inline void deallocate(Type * const data, const Range capacity) noexcept
        { Allocator::Deallocate(data, sizeof(Type) * std::size_t(capacity), alignof(Type)); }


private:
    Type *_data {};
    Range _size {};
    Range _capacity {};
};

#include "VectorBase.ipp"