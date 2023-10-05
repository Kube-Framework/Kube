/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>

#include "Assert.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    class IAllocator;

    namespace Internal
    {
        template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
        class VectorDetails;
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
class kF::Core::Internal::VectorDetails : public Base
{
public:
    /** @brief Static tag which indicates that the vector is not sorted */
    static constexpr bool IsSorted = false;

    /** @brief Static tag which indicates that the vector is not sorted */
    static constexpr bool HasRuntimeAllocator = IsRuntimeAllocated;

    /** @brief Iterator detectors */
    using ValueType = Type;

    /** @brief Type of the vector range */
    using RangeType = Range;

    /** @brief Iterator detectors */
    using Iterator = decltype(std::declval<Base &>().begin());
    using ConstIterator = decltype(std::declval<const Base &>().begin());

    /** @brief Iterators helper types */
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    /** @brief All required base functions */
    using Base::data;
    using Base::dataUnsafe;
    using Base::size;
    using Base::sizeUnsafe;
    using Base::capacity;
    using Base::capacityUnsafe;
    using Base::begin;
    using Base::beginUnsafe;
    using Base::end;
    using Base::endUnsafe;
    using Base::empty;
    using Base::steal;
    using Base::swap;
    using Base::isSafe;

    /** @brief Release the vector */
    inline ~VectorDetails(void) noexcept { release(); }


    /** @brief Default constructor */
    inline VectorDetails(void) noexcept
            requires (!IsRuntimeAllocated) = default;

    /** @brief Default constructor - Allocated version */
    inline VectorDetails(IAllocator &allocator) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) {}


    /** @brief Copy constructor */
    inline VectorDetails(const VectorDetails &other) noexcept
            requires (!IsRuntimeAllocated)
        { resize(other.begin(), other.end()); }

    /** @brief Copy constructor - Allocated version */
    inline VectorDetails(const VectorDetails &other) noexcept
            requires (IsRuntimeAllocated)
        : Base(other.allocator()) { resize(other.begin(), other.end()); }


    /** @brief Move constructor */
    inline VectorDetails(VectorDetails &&other) noexcept
        { steal(other); }


    /** @brief Resize with default constructor */
    inline VectorDetails(const Range count) noexcept
            requires (!IsRuntimeAllocated)
        { resize(count); }

    /** @brief Resize with default constructor - Allocated version */
    inline VectorDetails(IAllocator &allocator, const Range count) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(count); }


    /** @brief Resize with copy constructor */
    inline VectorDetails(const Range count, const Type &value) noexcept
            requires (std::copy_constructible<Type> && !IsRuntimeAllocated)
        { resize(count, value); }

    /** @brief Resize with copy constructor - Allocated version */
    inline VectorDetails(IAllocator &allocator, const Range count, const Type &value) noexcept
            requires (std::copy_constructible<Type> && IsRuntimeAllocated)
        : Base(allocator) { resize(count, value); }


    /** @brief Resize with initializer constructor */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    inline VectorDetails(const Range count, Initializer &&initializer) noexcept
        { resize(count, std::forward<Initializer>(initializer)); }

    /** @brief Resize with initializer - Allocated version */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    inline VectorDetails(IAllocator &allocator, const Range count, Initializer &&initializer) noexcept
        : Base(allocator) { resize(count, std::forward<Initializer>(initializer)); }


    /** @brief Resize constructor */
    template<std::input_iterator InputIterator>
    inline VectorDetails(const InputIterator from, const InputIterator to) noexcept
            requires (!IsRuntimeAllocated)
        { resize(from, to); }

    /** @brief Resize constructor - Allocated version */
    template<std::input_iterator InputIterator>
    inline VectorDetails(IAllocator &allocator, const InputIterator from, const InputIterator to) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(from, to); }


    /** @brief Resize map constructor */
    template<std::input_iterator InputIterator, typename Map>
    inline VectorDetails(const InputIterator from, const InputIterator to, Map &&map) noexcept
            requires (!IsRuntimeAllocated)
        { resize(from, to, std::forward<Map>(map)); }

    /** @brief Resize map constructor - Allocated version */
    template<std::input_iterator InputIterator, typename Map>
    inline VectorDetails(IAllocator &allocator, const InputIterator from, const InputIterator to, Map &&map) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(from, to, std::forward<Map>(map)); }


    /** @brief Initializer list constructor */
    inline VectorDetails(std::initializer_list<Type> &&init) noexcept
            requires (!IsRuntimeAllocated)
        : VectorDetails(init.begin(), init.end()) {}

    /** @brief Initializer list constructorr - Allocated version */
    inline VectorDetails(IAllocator &allocator, std::initializer_list<Type> &&init) noexcept
            requires (IsRuntimeAllocated)
        : VectorDetails(allocator, init.begin(), init.end()) {}


    /** @brief Copy assignment */
    inline VectorDetails &operator=(const VectorDetails &other) noexcept
        { resize(other.begin(), other.end()); return *this; }

    /** @brief Initializer list assignment */
    inline VectorDetails &operator=(std::initializer_list<Type> &&init) noexcept
        { resize(init.begin(), init.end()); return *this; }


    /** @brief Move assignment */
    inline VectorDetails &operator=(VectorDetails &&other) noexcept { steal(other); return *this; }


    /** @brief Fast non-empty check */
    [[nodiscard]] explicit inline operator bool(void) const noexcept { return !empty(); }


    /** @brief Begin / End helpers */
    [[nodiscard]] inline ConstIterator cbegin(void) const noexcept { return begin(); }
    [[nodiscard]] inline ConstIterator cend(void) const noexcept { return end(); }
    [[nodiscard]] inline ReverseIterator rbegin(void) noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] inline ReverseIterator rend(void) noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] inline ConstReverseIterator rbegin(void) const noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] inline ConstReverseIterator rend(void) const noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] inline ConstReverseIterator crbegin(void) const noexcept { return rbegin(); }
    [[nodiscard]] inline ConstReverseIterator crend(void) const noexcept { return rend(); }


    /** @brief Access element at positon */
    [[nodiscard]] inline Type &at(const Range pos) noexcept { return data()[pos]; }
    [[nodiscard]] inline const Type &at(const Range pos) const noexcept { return data()[pos]; }

    /** @brief Access element at positon */
    [[nodiscard]] inline Type &operator[](const Range pos) noexcept { return data()[pos]; }
    [[nodiscard]] inline const Type &operator[](const Range pos) const noexcept { return data()[pos]; }

    /** @brief Get first element */
    [[nodiscard]] inline Type &front(void) noexcept { return at(0); }
    [[nodiscard]] inline const Type &front(void) const noexcept { return at(0); }

    /** @brief Get last element */
    [[nodiscard]] inline Type &back(void) noexcept { return at(sizeUnsafe() - 1); }
    [[nodiscard]] inline const Type &back(void) const noexcept { return at(sizeUnsafe() - 1); }


    /** @brief Push an element into the vector */
    template<typename ...Args> requires std::constructible_from<Type, Args...>
    Type &push(Args &&...args) noexcept;

    /** @brief Pop the last element of the vector */
    void pop(void) noexcept;


    /** @brief Insert a range of copies */
    Iterator insertDefault(const Iterator pos, const Range count) noexcept;

    /** @brief Insert a range of copies */
    Iterator insertFill(const Iterator pos, const Range count, const Type &value) noexcept;

    /** @brief Insert a value by copy */
    inline Iterator insert(const Iterator pos, const Type &value) noexcept
        { return insert(pos, &value, &value + 1); }

    /** @brief Insert a value by move */
    inline Iterator insert(const Iterator pos, Type &&value) noexcept
        { return insert(pos, std::make_move_iterator(&value), std::make_move_iterator(&value + 1)); }

    /** @brief Insert an initializer list */
    inline Iterator insert(const Iterator pos, std::initializer_list<Type> &&init) noexcept
        { return insert(pos, init.begin(), init.end()); }

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator>
    Iterator insert(const Iterator pos, const InputIterator from, const InputIterator to) noexcept;

    /** @brief Insert a range of element by using a map function over iterators */
    template<std::input_iterator InputIterator, typename Map>
    Iterator insert(const Iterator pos, const InputIterator from, const InputIterator to, Map &&map) noexcept;

    /** @brief Insert a range of element by using a custom insert functor
     *  @note The functor must have the following signature: void(Range count, Iterator output) */
    template<typename InsertFunc>
    Iterator insertCustom(const Iterator pos, const Range count, InsertFunc &&insertFunc) noexcept;


    /** @brief Remove a range of elements */
    Iterator erase(Iterator from, Iterator to) noexcept;

    /** @brief Remove a range of elements */
    inline Iterator erase(Iterator from, const Range count) noexcept
        { return erase(from, from + count); }

    /** @brief Remove a specific element */
    inline Iterator erase(const Iterator pos) noexcept
        { return erase(pos, pos + 1); }


    /** @brief Resize the vector without constructing each element */
    void resizeUninitialized(const Range count) noexcept;

    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const Range count) noexcept
        requires std::constructible_from<Type>;

    /** @brief Resize the vector by copying given element */
    void resize(const Range count, const Type &value) noexcept
        requires std::copy_constructible<Type>;

    /** @brief Resize the vector by initializing each element with a functor
     *  @note The initializer functor can take an optional argument of type 'Range' as index */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    void resize(const Range count, Initializer &&initializer) noexcept;

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(const InputIterator from, const InputIterator to) noexcept;

    /** @brief Resize the vector using a map function with input iterators */
    template<std::input_iterator InputIterator, typename Map>
    void resize(const InputIterator from, const InputIterator to, Map &&map) noexcept;


    /** @brief Destroy all elements */
    void clear(void) noexcept;

    /** @brief Destroy all elements - Unsafe version*/
    void clearUnsafe(void) noexcept;


    /** @brief Destroy all elements and release the buffer instance */
    void release(void) noexcept;

    /** @brief Destroy all elements and release the buffer instance - Unsafe version */
    void releaseUnsafe(void) noexcept;


    /** @brief Reserve memory for fast emplace only if asked capacity is higher than current capacity
     *  The data is either preserved or moved  */
    void reserve(const Range capacity) noexcept;


    /** @brief Move range [from, to[ into out */
    void move(const Range from, const Range to, const Range output) noexcept;


    /** @brief Comparison operators */
    [[nodiscard]] bool operator==(const VectorDetails &other) const noexcept
        requires std::equality_comparable<Type>;
    [[nodiscard]] inline bool operator!=(const VectorDetails &other) const noexcept
        requires std::equality_comparable<Type>
        { return !operator==(other); }

    /** @brief Find an element by comparison */
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline Iterator find(const Comparable &comparable) noexcept
        { return std::find(begin(), end(), comparable); }
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ConstIterator find(const Comparable &comparable) const noexcept
        { return std::find(begin(), end(), comparable); }

    /** @brief Find an element by comparison, using begin iterator */
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline Iterator find(const Iterator from, const Comparable &comparable) noexcept
        { return std::find(from, end(), comparable); }
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ConstIterator find(const ConstIterator from, const Comparable &comparable) const noexcept
        { return std::find(from, end(), comparable); }

    /** @brief Find an element by comparison with reverse order */
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ReverseIterator rfind(const Comparable &comparable) noexcept
        { return std::find(rbegin(), rend(), comparable); }
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ConstReverseIterator rfind(const Comparable &comparable) const noexcept
        { return std::find(rbegin(), rend(), comparable); }

    /** @brief Find an element by comparison with reverse order, using begin iterator */
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ReverseIterator rfind(const ReverseIterator from, const Comparable &comparable) noexcept
        { return std::find(from, rend(), comparable); }
    template<typename Comparable>
        requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ConstReverseIterator rfind(const ConstReverseIterator from, const Comparable &comparable) const noexcept
        { return std::find(from, rend(), comparable); }

    /** @brief Find an element with functor */
    template<typename Functor>
        requires std::invocable<Functor, Type &>
    [[nodiscard]] inline Iterator find(Functor &&functor) noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const Type &>
    [[nodiscard]] inline ConstIterator find(Functor &&functor) const noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor, using begin iterator */
    template<typename Functor>
        requires std::invocable<Functor, Type &>
    [[nodiscard]] inline Iterator find(const Iterator from, Functor &&functor) noexcept
        { return std::find_if(from, end(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const Type &>
    [[nodiscard]] inline ConstIterator find(const ConstIterator from, Functor &&functor) const noexcept
        { return std::find_if(from, end(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor with reverse order */
    template<typename Functor>
        requires std::invocable<Functor, Type &>
    [[nodiscard]] inline ReverseIterator rfind(Functor &&functor) noexcept
        { return std::find_if(rbegin(), rend(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const Type &>
    [[nodiscard]] inline ConstReverseIterator rfind(Functor &&functor) const noexcept
        { return std::find_if(rbegin(), rend(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor with reverse order, using reversed begin iterator */
    template<typename Functor>
        requires std::invocable<Functor, Type &>
    [[nodiscard]] inline ReverseIterator rfind(const ReverseIterator from, Functor &&functor) noexcept
        { return std::find_if(from, rend(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const Type &>
    [[nodiscard]] inline ConstReverseIterator rfind(const ConstReverseIterator from, Functor &&functor) const noexcept
        { return std::find_if(from, rend(), std::forward<Functor>(functor)); }


    /** @brief Grow internal buffer of a given minimum */
    void grow(const Range minimum) noexcept;


    /** @brief Sort the vector with default Type comparison operator */
    inline void sort(void) noexcept
        { std::sort(begin(), end()); }

    /** @brief Sort the vector using a comparator functor */
    template<typename Compare>
    inline void sort(Compare &&compare) noexcept { std::sort(begin(), end(), std::forward<Compare>(compare)); }



    /** @brief Implicitly convert instance into an output IteratorRange */
    [[nodiscard]] inline operator Core::IteratorRange<Iterator>(void) noexcept
        { return Core::IteratorRange<Iterator> { begin(), end() }; }

    /** @brief Implicitly convert instance into an input IteratorRange */
    [[nodiscard]] inline operator Core::IteratorRange<ConstIterator>(void) const noexcept
        { return Core::IteratorRange<ConstIterator> { begin(), end() }; }


    /** @brief Convert instance into an output IteratorRange */
    [[nodiscard]] inline Core::IteratorRange<Iterator> toRange(void) noexcept
        { return Core::IteratorRange<Iterator> { begin(), end() }; }

    /** @brief Convert instance into an input IteratorRange */
    [[nodiscard]] inline Core::IteratorRange<ConstIterator> toRange(void) const noexcept
        { return Core::IteratorRange<ConstIterator> { begin(), end() }; }


    /** @brief Get the index of an iterator */
    [[nodiscard]] inline Range indexOf(const ConstIterator pos) const noexcept
        { return static_cast<Range>(std::distance(begin(), pos)); }

protected:
    using Base::setSize;
    using Base::setData;
    using Base::setCapacity;
    using Base::allocate;
    using Base::deallocate;

    /** @brief Reserve unsafe takes IsSafe as template parameter */
    template<bool IsSafe = true, bool IsPreserved = true>
    void reserveUnsafe(const Range capacity) noexcept;
};

#include "VectorDetails.ipp"