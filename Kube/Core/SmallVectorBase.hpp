/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SmallVectorBase
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
    class SmallVectorBase;

    template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range, typename IntrusiveBase>
    class IntrusiveSmallVectorBase;
}

/** @brief Base implementation of a vector with size and capacity cached and a small optimization */
template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
class kF::Core::Internal::SmallVectorBase
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


    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] inline Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] inline Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] inline ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] inline ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }

    /** @brief Begin / end overloads */
    [[nodiscard]] inline Iterator begin(void) noexcept { return beginUnsafe(); }
    [[nodiscard]] inline Iterator end(void) noexcept { return endUnsafe(); }
    [[nodiscard]] inline ConstIterator begin(void) const noexcept { return beginUnsafe(); }
    [[nodiscard]] inline ConstIterator end(void) const noexcept { return endUnsafe(); }


    /** @brief Steal another instance */
    void steal(SmallVectorBase &other) noexcept;

    /** @brief Swap two instances */
    void swap(SmallVectorBase &other) noexcept;


    /** @brief Tell if the vector currently use its cache or a heap allocation*/
    [[nodiscard]] inline bool isCacheUsed(void) const noexcept
        { return _data == optimizedData(); }

protected:
    /** @brief Protected data setter */
    inline void setData(Type * const data) noexcept { _data = data; }

    /** @brief Protected size setter */
    inline void setSize(const Range size) noexcept { _size = size; }

    /** @brief Protected capacity setter */
    inline void setCapacity(const Range capacity) noexcept { _capacity = capacity; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept;


    /** @brief Get a pointer to the data cache */
    [[nodiscard]] inline Type *optimizedData(void) noexcept
        { return reinterpret_cast<Type *>(&_optimizedData); }
    [[nodiscard]] inline const Type *optimizedData(void) const noexcept
        { return reinterpret_cast<const Type *>(&_optimizedData); }

private:
    Type *_data {};
    Range _size {};
    Range _capacity {};
    alignas(alignof(Type)) std::byte _optimizedData[sizeof(Type) * OptimizedCapacity];
};

#include "SmallVectorBase.ipp"

/** @brief Base implementation of a vector with size and capacity cached and a small optimization
 *  This class inherits from both SmallVectorBase and IntrusiveBase */
template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range,
        typename IntrusiveBase>
class kF::Core::Internal::IntrusiveSmallVectorBase
        : protected IntrusiveBase, public SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>
{
};