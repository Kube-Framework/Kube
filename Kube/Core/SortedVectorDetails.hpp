/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVectorDetails
 */

#pragma once


#include "VectorDetails.hpp"

namespace kF::Core
{
    class IAllocator;

    namespace Internal
    {
        template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
        class SortedVectorDetails;
    }
}

/** @brief Implementation details of any sorted vector.
 *  The compare function must not throw ! */
template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
class kF::Core::Internal::SortedVectorDetails : private VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>
{
public:
    /** @brief Static tag which indicates that the vector is sorted */
    static constexpr bool IsSorted = true;

    /** @brief Type alias to VectorDetails */
    using DetailsBase = VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>;

    /** @brief Iterator detectors */
    using Iterator = typename DetailsBase::Iterator;
    using ConstIterator = typename DetailsBase::ConstIterator;


    /** @brief Release the vector */
    inline ~SortedVectorDetails(void) noexcept = default;


    /** @brief Default constructor */
    inline SortedVectorDetails(void) noexcept requires (!IsRuntimeAllocated) = default;

    /** @brief Default constructor - Allocated version */
    inline SortedVectorDetails(IAllocator &allocator) noexcept
            requires (IsRuntimeAllocated)
        : DetailsBase(allocator) {}


    /** @brief Copy constructor */
    inline SortedVectorDetails(const SortedVectorDetails &other) noexcept = default;

    /** @brief Move constructor */
    inline SortedVectorDetails(SortedVectorDetails &&other) noexcept = default;


    /** @brief Resize with default constructor */
    inline SortedVectorDetails(const Range count) noexcept
            requires (!IsRuntimeAllocated)
        { resize(count); }

    /** @brief Resize with default constructor - Allocated version */
    inline SortedVectorDetails(IAllocator &allocator, const Range count) noexcept
            requires (IsRuntimeAllocated)
        : DetailsBase(allocator) { resize(count); }


    /** @brief Resize with copy constructor */
    inline SortedVectorDetails(const Range count, const Type &value) noexcept
            requires (std::copy_constructible<Type> && !IsRuntimeAllocated)
        { resize(count, value); }

    /** @brief Resize with copy constructor - Allocated version */
    inline SortedVectorDetails(IAllocator &allocator, const Range count, const Type &value) noexcept
            requires (std::copy_constructible<Type> && IsRuntimeAllocated)
        : DetailsBase(allocator) { resize(count, value); }


    /** @brief Resize with initializer constructor */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    inline SortedVectorDetails(const Range count, Initializer &&initializer) noexcept
        { resize(count, std::forward<Initializer>(initializer)); }

    /** @brief Resize with initializer - Allocated version */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    inline SortedVectorDetails(IAllocator &allocator, const Range count, Initializer &&initializer) noexcept
        : DetailsBase(allocator) { resize(count, std::forward<Initializer>(initializer)); }


    /** @brief Resize constructor */
    template<std::input_iterator InputIterator>
    inline SortedVectorDetails(const InputIterator from, const InputIterator to) noexcept
            requires (!IsRuntimeAllocated)
        { resize(from, to); }

    /** @brief Resize constructor - Allocated version */
    template<std::input_iterator InputIterator>
    inline SortedVectorDetails(IAllocator &allocator, const InputIterator from, const InputIterator to) noexcept
            requires (IsRuntimeAllocated)
        : DetailsBase(allocator) { resize(from, to); }


    /** @brief Resize map constructor */
    template<std::input_iterator InputIterator, typename Map>
    inline SortedVectorDetails(const InputIterator from, const InputIterator to, Map &&map) noexcept
            requires (!IsRuntimeAllocated)
        { resize(from, to, std::forward<Map>(map)); }

    /** @brief Resize map constructor - Allocated version */
    template<std::input_iterator InputIterator, typename Map>
    inline SortedVectorDetails(IAllocator &allocator, const InputIterator from, const InputIterator to, Map &&map) noexcept
            requires (IsRuntimeAllocated)
        : DetailsBase(allocator) { resize(from, to, std::forward<Map>(map)); }


    /** @brief Initializer list constructor */
    inline SortedVectorDetails(std::initializer_list<Type> &&init) noexcept
            requires (!IsRuntimeAllocated)
        : SortedVectorDetails(init.begin(), init.end()) {}

    /** @brief Initializer list constructor - Allocated version */
    inline SortedVectorDetails(IAllocator &allocator, std::initializer_list<Type> &&init) noexcept
            requires (IsRuntimeAllocated)
        : SortedVectorDetails(allocator, init.begin(), init.end()) {}


    /** @brief Copy assignment */
    inline SortedVectorDetails &operator=(const SortedVectorDetails &other) noexcept
        { DetailsBase::resize(other.begin(), other.end()); return *this; }

    /** @brief Move assignment */
    inline SortedVectorDetails &operator=(SortedVectorDetails &&other) noexcept
        { DetailsBase::steal(other); return *this; }

    /** @brief Push an element into the vector */
    template<typename ...Args> requires std::constructible_from<Type, Args...>
    Type &push(Args &&...args) noexcept;


    /** @brief Insert a range of default initialized values */
    Iterator insertDefault(const Range count) noexcept;

    /** @brief Insert a range of copies */
    Iterator insertFill(const Range count, const Type &value) noexcept;

    /** @brief Insert an initializer list */
    inline void insert(std::initializer_list<Type> &&init) noexcept
        { return insert(init.begin(), init.end()); }

    /** @brief Insert an value by copy, returning its iterator */
    inline Iterator insert(const Type &value) noexcept
        { return &push(value); }

    /** @brief Insert an value by move, returning its iterator */
    inline Iterator insert(Type &&value) noexcept
        { return &push(std::move(value)); }

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator>
    void insert(const InputIterator from, const InputIterator to) noexcept;

    /** @brief Insert a range of element by using a map function over iterators */
    template<std::input_iterator InputIterator, typename Map>
    void insert(const InputIterator from, const InputIterator to, Map &&map) noexcept;

    /** @brief Insert a range of element by using a custom insert functor */
    template<typename InsertFunc>
    void insertCustom(const Range count, InsertFunc &&insertFunc) noexcept;


    /** @brief Insert an value by copy at a specific location (no sort involved), returning its iterator */
    inline Iterator insertAt(const Iterator at, const Type &value) noexcept
        { return DetailsBase::insert(at, value); }

    /** @brief Insert an value by move at a specific location (no sort involved), returning its iterator */
    inline Iterator insertAt(const Iterator at, Type &&value) noexcept
        { return DetailsBase::insert(at, std::move(value)); }


    /** @brief Resize the vector using default constructor to initialize each element */
    inline void resize(const Range count) noexcept
        requires std::constructible_from<Type>
        { DetailsBase::resize(count); }

    /** @brief Resize the vector by copying given element */
    inline void resize(const Range count, const Type &type) noexcept
        requires std::copy_constructible<Type>
        { DetailsBase::resize(count, type); }

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(const InputIterator from, const InputIterator to) noexcept;

    /** @brief Resize the vector using a map function with input iterators */
    template<std::input_iterator InputIterator, typename Map>
    void resize(const InputIterator from, const InputIterator to, Map &&map) noexcept;

    /** @brief Resize the vector by initializing each element with a functor
     *  @note The initializer functor can take an optional argument of type 'Range' as index */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
    void resize(const Range count, Initializer &&initializer) noexcept;


    /** @brief Sort the vector */
    inline void sort(void) noexcept { std::sort(DetailsBase::begin(), DetailsBase::end(), Compare{}); }

    /** @brief Assign a new value to an existing element
     *  @return The index where the element has been moved if assignment break sort */
    template<typename AssignType>
    Range assign(const Range index, AssignType &&value) noexcept;


    /** @brief Finds where to insert an element */
    [[nodiscard]] inline Iterator findSortedPlacement(const Type &value) noexcept
        { return DetailsBase::find([&value](const Type &other) { return Compare{}(value, other); }); }
    [[nodiscard]] inline ConstIterator findSortedPlacement(const Type &value) const noexcept
        { return DetailsBase::find([&value](const Type &other) { return Compare{}(value, other); }); }

    /** @brief Comparison operators */
    [[nodiscard]] inline bool operator==(const SortedVectorDetails &other) const noexcept = default;
    [[nodiscard]] inline bool operator!=(const SortedVectorDetails &other) const noexcept = default;


    /** @brief All required base functions - FixMSVCPlz */
    using DetailsBase::data;
    using DetailsBase::dataUnsafe;
    using DetailsBase::size;
    using DetailsBase::sizeUnsafe;
    using DetailsBase::capacity;
    using DetailsBase::capacityUnsafe;
    using DetailsBase::begin;
    using DetailsBase::beginUnsafe;
    using DetailsBase::end;
    using DetailsBase::endUnsafe;
    using DetailsBase::empty;
    using DetailsBase::steal;
    using DetailsBase::swap;
    using DetailsBase::isSafe;
    using DetailsBase::operator bool;
    using DetailsBase::cbegin;
    using DetailsBase::cend;
    using DetailsBase::rbegin;
    using DetailsBase::rend;
    using DetailsBase::crbegin;
    using DetailsBase::crend;
    using DetailsBase::at;
    using DetailsBase::operator[];
    using DetailsBase::front;
    using DetailsBase::back;
    using DetailsBase::erase;
    using DetailsBase::resizeUninitialized;
    using DetailsBase::clear;
    using DetailsBase::clearUnsafe;
    using DetailsBase::release;
    using DetailsBase::releaseUnsafe;
    using DetailsBase::reserve;
    using DetailsBase::move;
    using DetailsBase::operator==;
    using DetailsBase::operator!=;
    using DetailsBase::find;
    using DetailsBase::rfind;
    using DetailsBase::grow;
    using DetailsBase::toRange;
    using DetailsBase::indexOf;
};

#include "SortedVectorDetails.ipp"