/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A dead simple runtime array
 */

#pragma once

#include <memory>

#include "Assert.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
    class HeapArray;

    /** @brief Heap array allow fixed size allocated array and a long range */
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
    using LongHeapArray = HeapArray<Type, Allocator, std::size_t>;
}

/** @brief Heap array allow fixed size allocated array */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator, std::integral Range = std::uint32_t>
class alignas_quarter_cacheline kF::Core::HeapArray
{
public:
    /** @brief Array iterator */
    using Iterator = Type *;

    /** @brief Array constant iterator */
    using ConstIterator = const Type *;


    /** @brief Destruct the array and all elements */
    inline ~HeapArray(void) noexcept { release(); }

    /** @brief Default construct an empty array */
    inline HeapArray(void) noexcept = default;

    /** @brief Construct an array of a given size with given arguments */
    template<typename ...Args>
    inline HeapArray(const Range size, Args &&...args) noexcept
        { allocateUnsafe(size, args...); }

    /** @brief Copy constructor */
    inline HeapArray(const HeapArray &other) noexcept : HeapArray(other._size) { std::copy(other.begin(), other.end(), begin()); }

    /** @brief Move constructor */
    inline HeapArray(HeapArray &&other) noexcept
        : _data(other._data), _size(other._size) { other._data = nullptr; other._size = 0ul; }

    /** @brief Copy assignment */
    inline HeapArray &operator=(const HeapArray &other) noexcept { allocate(other._size); std::copy(other.begin(), other.end(), begin()); }

    /** @brief Move assignment */
    inline HeapArray &operator=(HeapArray &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    inline void swap(HeapArray &other) noexcept { std::swap(_data, other._data); std::swap(_size, other._size); }


    /** @brief Non-empty check */
    inline operator bool(void) const noexcept { return _size; }

    /** @brief Empty check */
    [[nodiscard]] inline bool empty(void) const noexcept { return !_size; }


    /** @brief Allocate a new array */
    template<typename ...Args>
        requires std::constructible_from<Type, Args...>
    inline void allocate(const Range size, Args ...args) noexcept
        { release(); allocateUnsafe(size, std::forward<Args>(args)...); }

    /** @brief Clear the array and release memory */
    void release(void) noexcept;


    /** @brief Get internal data */
    [[nodiscard]] inline Type *data(void) noexcept { return _data; }

    /** @brief Get internal constant data */
    [[nodiscard]] inline const Type *data(void) const noexcept { return _data; }


    /** @brief Get array length */
    [[nodiscard]] inline Range size(void) const noexcept { return _size; }


    /** @brief Dereference at */
    [[nodiscard]] inline Type &at(const Range index) noexcept { return _data[index]; }

    /** @brief Constant dereference at */
    [[nodiscard]] inline const Type &at(const Range index) const noexcept { return _data[index]; }


    /** @brief Dereference at */
    [[nodiscard]] inline Type &operator[](const Range index) noexcept { return _data[index]; }

    /** @brief Constant dereference at */
    [[nodiscard]] inline const Type &operator[](const Range index) const noexcept { return _data[index]; }


    /** @brief Begin iterators */
    [[nodiscard]] inline Iterator begin(void) noexcept { return _data; }

    /** @brief End iterators */
    [[nodiscard]] inline Iterator end(void) noexcept { return _data + _size; }

    /** @brief Constant begin iterators */
    [[nodiscard]] inline ConstIterator begin(void) const noexcept { return _data; }

    /** @brief Constant end iterators */
    [[nodiscard]] inline ConstIterator end(void) const noexcept { return _data + _size; }

    /** @brief Constant begin iterators */
    [[nodiscard]] inline ConstIterator cbegin(void) const noexcept { return _data; }

    /** @brief Constant end iterators */
    [[nodiscard]] inline ConstIterator cend(void) const noexcept { return _data + _size; }

private:
    Type *_data {};
    Range _size { 0 };

    /** @brief Unsafe version of the allocate function */
    template<typename ...Args>
        requires std::constructible_from<Type, Args...>
    void allocateUnsafe(const Range size, Args ...args) noexcept;
};

#include "HeapArray.ipp"
