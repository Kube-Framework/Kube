/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: StringDetails
 */

#pragma once

#include <string_view>
#include <cstring>

#include "Utils.hpp"

namespace kF::Core
{
    class IAllocator;

    namespace Internal
    {
        template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
            requires std::is_trivially_copyable_v<Type>
        class StringDetails;

        /** @brief Strlen but with null cstring check */
        template<std::integral Range, typename Type>
        [[nodiscard]] static Range SafeStrlen(const Type * const cstring) noexcept;
    }
}

/** @brief String details bring facilities to manipulate a vector as a string */
template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
class kF::Core::Internal::StringDetails : public Base
{
public:
    /** @brief View */
    using View = std::basic_string_view<Type>;

    using Base::Base;
    using Base::data;
    using Base::dataUnsafe;
    using Base::size;
    using Base::sizeUnsafe;
    using Base::capacity;
    using Base::capacityUnsafe;
    using Base::begin;
    using Base::end;
    using Base::resize;
    using Base::insert;
    using Base::isSafe;
    using Base::reserve;
    using Base::grow;
    using Base::operator bool;


    /** @brief Destructor */
    inline ~StringDetails(void) noexcept = default;


    /** @brief Default constructor */
    inline StringDetails(void) noexcept
            requires (!IsRuntimeAllocated) = default;

    /** @brief Default constructor - Allocated version*/
    inline StringDetails(IAllocator &allocator) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) {}


    /** @brief Copy constructor */
    inline StringDetails(const StringDetails &other) noexcept = default;

    /** @brief Move constructor */
    inline StringDetails(StringDetails &&other) noexcept = default;


    /** @brief CString constructor */
    inline StringDetails(const char * const cstring) noexcept
            requires (!IsRuntimeAllocated)
        { resize(cstring, cstring + SafeStrlen<Range>(cstring)); }

    /** @brief CString constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const char * const cstring) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(cstring, cstring + SafeStrlen<Range>(cstring)); }


    /** @brief CString length constructor */
    inline StringDetails(const char * const cstring, const std::size_t length) noexcept
            requires (!IsRuntimeAllocated)
        { resize(cstring, cstring + length); }

    /** @brief CString length constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const char * const cstring, const std::size_t length) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(cstring, cstring + length); }


    /** @brief View constructor */
    inline StringDetails(const View &other) noexcept
            requires (!IsRuntimeAllocated)
        { resize(other.begin(), other.end()); }

    /** @brief View constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const View &other) noexcept
            requires (IsRuntimeAllocated)
        : Base(allocator) { resize(other.begin(), other.end()); }


    /** @brief Copy assignment */
    inline StringDetails &operator=(const StringDetails &other) noexcept = default;

    /** @brief Move assignment */
    inline StringDetails &operator=(StringDetails &&other) noexcept = default;

    /** @brief cstring assignment */
    inline StringDetails &operator=(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen<Range>(cstring)); return *this; }

    /** @brief View assignment */
    inline StringDetails &operator=(const View &other) noexcept { resize(other.begin(), other.end()); return *this; }


    /** @brief Get string length (alias of 'size' function) */
    [[nodiscard]] Range length(void) const noexcept { return size(); }
    [[nodiscard]] Range lengthUnsafe(void) const noexcept { return sizeUnsafe(); }


    /** @brief Comparison operator */
    [[nodiscard]] inline bool operator==(const StringDetails &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] inline bool operator!=(const StringDetails &other) const noexcept { return !operator==(other); }
    [[nodiscard]] inline bool operator==(const char * const cstring) const noexcept { return std::equal(begin(), end(), cstring, cstring + SafeStrlen<Range>(cstring)); }
    [[nodiscard]] inline bool operator!=(const char * const cstring) const noexcept { return !operator==(cstring); }
    [[nodiscard]] inline bool operator==(const View &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] inline bool operator!=(const View &other) const noexcept { return !operator==(other); }


    /** @brief Append operator */
    inline StringDetails &operator+=(const StringDetails &other) noexcept { this->insert(end(), other.begin(), other.end()); return *this; }
    inline StringDetails &operator+=(const Type character) noexcept { this->push(character); return *this; }
    inline StringDetails &operator+=(const char * const cstring) noexcept { this->insert(end(), cstring, cstring + SafeStrlen<Range>(cstring)); return *this; }
    inline StringDetails &operator+=(const View &other) noexcept { this->insert(end(), other.begin(), other.end()); return *this; }
    template<typename OtherBase, typename OtherType, std::integral OtherRange, bool OtherIsRuntimeAllocated>
        requires std::is_trivially_copyable_v<OtherType>
    inline StringDetails &operator+=(const StringDetails<OtherBase, OtherType, OtherRange, OtherIsRuntimeAllocated> &other) noexcept
        { return *this += other.toView(); }


    /** @brief Addition operator */
    [[nodiscard]] inline StringDetails operator+(const StringDetails &other) const noexcept;
    [[nodiscard]] inline StringDetails operator+(const Type character) const noexcept;
    [[nodiscard]] inline StringDetails operator+(const char * const cstring) const noexcept;
    [[nodiscard]] inline StringDetails operator+(const View &other) const noexcept;


    /** @brief Implicit converted to std::string_view */
    [[nodiscard]] inline operator View(void) const noexcept { return toView(); }

    /** @brief Get a std::string_view from the object */
    [[nodiscard]] inline View toView(void) const noexcept { return isSafe() ? View(data(), sizeUnsafe()) : View(); }

    /** @brief Get a null terminated char array pointer
     *  ! Be careful as the function is constant for convinience but it can still modify the internal pointer ! */
    [[nodiscard]] const char *c_str(void) const noexcept
    { // MSVC Throws an internal error when compiling this inside 'ipp'
        if (!size())
            return nullptr;
        else if (size() == capacity())
            const_cast<StringDetails *>(this)->grow(1);
        *const_cast<StringDetails *>(this)->end() = '\0';
        return dataUnsafe();
    }


    /** @brief Check if substring is contained in string */
    [[nodiscard]] inline bool contains(const View &view) const noexcept { return toView().contains(view); }

    /** @brief Check if substring is at begin of string */
    [[nodiscard]] inline bool startsWith(const View &view) const noexcept { return toView().substr(0, view.size()) == view; }

    /** @brief Check if substring is at end of string */
    [[nodiscard]] inline bool endsWith(const View &view) const noexcept { return toView().substr(size() - view.size()) == view; }
};

/** @brief Additional addition operators */
template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
[[nodiscard]] kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const Type lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept;
template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
[[nodiscard]] kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const Type * const lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept;
template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
[[nodiscard]] kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const typename kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::View &lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept;

#include "StringDetails.ipp"
