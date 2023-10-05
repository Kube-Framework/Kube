/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: TagAlign pointer
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t Alignment>
    class TaggedPtr;
}

/** @brief TaggedPtr is a pointer that takes advantage of type alignment to use the lower bits of the pointer as a tag */
template<typename Type, std::size_t Alignment = alignof(Type)>
class alignas_eighth_cacheline kF::Core::TaggedPtr
{
public:
    static_assert(IsPowerOf2(Alignment), "TaggedPtr requires a valid alignment");
    static_assert(Alignment > 1, "TaggedPtr requires at least 1 bit of free space");

    static constexpr std::uintptr_t Max     = Alignment - 1;
    static constexpr std::uintptr_t TagMask = Max;
    static constexpr std::uintptr_t PtrMask = ~TagMask;

    /** @brief Destructor */
    inline ~TaggedPtr(void) noexcept = default;

    /** @brief Default constructor */
    constexpr TaggedPtr(void) noexcept = default;

    /** @brief Copy constructor */
    constexpr TaggedPtr(const TaggedPtr &other) noexcept = default;

    /** @brief Move constructor */
    constexpr TaggedPtr(TaggedPtr &&other) noexcept = default;


    /** @brief Pointer constructor */
    constexpr explicit TaggedPtr(Type * const ptr) noexcept
        : _data(reinterpret_cast<std::uintptr_t>(ptr)) {}

    /** @brief Pointer and tag constructor */
    constexpr TaggedPtr(Type * const ptr, const std::uintptr_t tag) noexcept
        : _data(reinterpret_cast<std::uintptr_t>(ptr) | (tag & TagMask)) {}


    /** @brief Copy assignment */
    constexpr TaggedPtr &operator=(const TaggedPtr &other) noexcept = default;

    /** @brief Move assignment */
    constexpr TaggedPtr &operator=(TaggedPtr &&other) noexcept = default;


    /** @brief Boolean operator */
    [[nodiscard]] constexpr operator bool(void) const noexcept { return get(); }


    /** @brief Acces<s operator */
    [[nodiscard]] constexpr Type *operator->(void) const noexcept { return get(); }

    /** @brief Dereference operator */
    [[nodiscard]] constexpr Type &operator*(void) const noexcept { return *get(); }


    /** @brief Get the pointer */
    [[nodiscard]] constexpr Type *get(void) const noexcept
        { return reinterpret_cast<Type *>(_data & PtrMask); }


    /** @brief Set the pointer */
    constexpr void set(const Type *ptr) noexcept
        { _data = reinterpret_cast<std::uintptr_t>(ptr) | (_data & TagMask); }

    /** @brief Set the pointer and the tag */
    constexpr void set(const Type *ptr, const std::uintptr_t tag) noexcept
        { _data = reinterpret_cast<std::uintptr_t>(ptr) | (tag & TagMask); }


    /** @brief Get the tag */
    [[nodiscard]] constexpr std::uintptr_t tag(void) const noexcept
        { return _data & TagMask; }

    /** @brief Set the tag */
    constexpr void setTag(const std::uintptr_t tag) noexcept
        { _data = (_data & PtrMask) | (tag & TagMask); }


    /** @brief Reset pointer and tag */
    void reset(void) noexcept { _data = static_cast<std::uintptr_t>(0); }


    /** @brief Equality operator */
    [[nodiscard]] constexpr bool operator==(const TaggedPtr &other) const noexcept
        { return _data == other._data; }

    /** @brief Inequality operator */
    [[nodiscard]] constexpr bool operator!=(const TaggedPtr &other) const noexcept
        { return _data != other._data; }

    /** @brief Nullptr equality operator */
    [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return get() == nullptr; }

    /** @brief Nullptr inequality operator */
    [[nodiscard]] constexpr bool operator!=(std::nullptr_t) const noexcept { return get() != nullptr; }


    /** @brief Inferior operator */
    [[nodiscard]] constexpr bool operator<(const TaggedPtr &other) const noexcept
        { return get() < other.get(); }

    /** @brief Inferior equal operator */
    [[nodiscard]] constexpr bool operator<=(const TaggedPtr &other) const noexcept
        { return get() <= other.get(); }


    /** @brief Superior operator */
    [[nodiscard]] constexpr bool operator>(const TaggedPtr &other) const noexcept
        { return get() > other.get(); }

    /** @brief Superior equal operator */
    [[nodiscard]] constexpr bool operator>=(const TaggedPtr &other) const noexcept
        { return get() >= other.get(); }


private:
    std::uintptr_t _data {};
};

/** @brief Reversed nullptr equality operator */
template<typename Type, std::size_t Alignment>
[[nodiscard]] constexpr bool operator==(std::nullptr_t, const kF::Core::TaggedPtr<Type, Alignment> &rhs) noexcept
    { return rhs == nullptr; }

/** @brief Reversed nullptr inequality operator */
template<typename Type, std::size_t Alignment>
[[nodiscard]] constexpr bool operator!=(std::nullptr_t, const kF::Core::TaggedPtr<Type, Alignment> &rhs) noexcept
    { return rhs != nullptr; }