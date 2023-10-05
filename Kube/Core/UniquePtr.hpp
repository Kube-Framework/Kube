/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unique pointer using allocator
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
    class UniquePtr;
}

/** @brief Unique pointer class */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class kF::Core::UniquePtr
{
public:
    /** @brief Allocate an instance */
    template<typename ...Args>
    [[nodiscard]] static inline UniquePtr Make(Args &&...args) noexcept
        { return UniquePtr(new (Allocator::Allocate(sizeof(Type), alignof(Type))) Type(std::forward<Args>(args)...)); }

    /** @brief Allocate an instance */
    template<typename Derived, typename ...Args>
        requires std::derived_from<Derived, Type>
    [[nodiscard]] static inline UniquePtr Make(Args &&...args) noexcept
        { return UniquePtr(new (Allocator::Allocate(sizeof(Derived), alignof(Derived))) Derived(std::forward<Args>(args)...)); }


    /** @brief Destructor */
    inline ~UniquePtr(void) noexcept { release(); }

    /** @brief Construct the unique pointer */
    inline UniquePtr(void) noexcept = default;

    /** @brief UniquePtr is not copiable */
    UniquePtr(const UniquePtr &other) noexcept = delete;
    UniquePtr &operator=(const UniquePtr &other) noexcept = delete;

    /** @brief Move constructor */
    inline UniquePtr(UniquePtr &&other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }

    /** @brief Move assignment */
    inline UniquePtr &operator=(UniquePtr &&other) noexcept
        { std::swap(_ptr, other._ptr); return *this; }


    /** @brief Move constructor from derived*/
    template<typename Other>
        requires std::derived_from<Other, Type>
    inline UniquePtr(UniquePtr<Other, Allocator> &&other) noexcept : _ptr(other.steal()) {}

    /** @brief Move assignment from derived */
    template<typename Other>
        requires std::derived_from<Other, Type>
    inline UniquePtr &operator=(UniquePtr<Other, Allocator> &&other) noexcept
        { release(); _ptr = other.steal(); return *this; }


    /** @brief Boolean check operator */
    [[nodiscard]] explicit inline operator bool(void) const noexcept { return _ptr != nullptr; }

    /** @brief Boolean check operator */
    [[nodiscard]] explicit inline operator Type *(void) noexcept { return get(); }
    [[nodiscard]] explicit inline operator const Type *(void) const noexcept { return get(); }


    /** @brief Mutable managed object getter */
    [[nodiscard]] inline Type *get(void) noexcept { return _ptr; }

    /** @brief Constant managed object getter */
    [[nodiscard]] inline const Type *get(void) const noexcept { return _ptr; }


    /** @brief Mutable managed object pointer access */
    [[nodiscard]] inline Type *operator->(void) noexcept { return _ptr; }

    /** @brief Constant managed object pointer access */
    [[nodiscard]] inline const Type *operator->(void) const noexcept { return _ptr; }


    /** @brief Mutable managed object reference access */
    [[nodiscard]] inline Type &operator*(void) noexcept { return *_ptr; }

    /** @brief Constant managed object reference access */
    [[nodiscard]] inline const Type &operator*(void) const noexcept { return *_ptr; }


    /** @brief Swaps the managed objects */
    inline void swap(UniquePtr &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Destroy the managed object */
    inline void release(void) noexcept { if (_ptr) releaseUnsafe(); }


    /** @brief Steal internal pointer, the object must be freed manually */
    [[nodiscard]] inline Type *steal(void) noexcept { const auto ptr = _ptr; _ptr = nullptr; return ptr; }


    /** @brief Comparison operators */
    [[nodiscard]] inline bool operator==(const UniquePtr &other) const noexcept { return _ptr == other._ptr; }
    [[nodiscard]] inline bool operator!=(const UniquePtr &other) const noexcept { return _ptr != other._ptr; }
    [[nodiscard]] inline bool operator==(const Type * const other) const noexcept { return _ptr == other; }
    [[nodiscard]] inline bool operator!=(const Type * const other) const noexcept { return _ptr != other; }
    [[nodiscard]] inline bool operator==(Type * const other) const noexcept { return _ptr == other; }
    [[nodiscard]] inline bool operator!=(Type * const other) const noexcept { return _ptr != other; }

private:
    /** @brief Instance constructor */
    inline UniquePtr(Type * const ptr) : _ptr(ptr) {}

    /** @brief Unsafe implementation of the release function */
    inline void releaseUnsafe(void) noexcept { _ptr->~Type(); Allocator::Deallocate(_ptr, sizeof(Type), alignof(Type)); _ptr = nullptr; }


    Type *_ptr {};
};