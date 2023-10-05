/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Shared pointer using allocator
 */

#pragma once

#include <atomic>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
    class SharedPtr;
}

/** @brief Shared pointer class */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class kF::Core::SharedPtr
{
public:
    /** @brief Allocated data of the shared pointer */
    struct Data
    {
        Type value {};
        std::atomic<std::uint32_t> count { 1u };


        /** @brief Destructor */
        inline ~Data(void) noexcept = default;

        /** @brief Type constructor */
        template<typename ...Args>
        inline Data(Args &&...args) noexcept : value(std::forward<Args>(args)...) {}
    };

    /** @brief Allocate an instance */
    template<typename ...Args>
    [[nodiscard]] static inline SharedPtr Make(Args &&...args) noexcept
        { return SharedPtr(new (Allocator::Allocate(sizeof(Data), alignof(Data))) Data(std::forward<Args>(args)...)); }


    /** @brief Destructor */
    inline ~SharedPtr(void) noexcept { release<false>(); }

    /** @brief Construct the shared pointer */
    inline SharedPtr(void) noexcept = default;

    /** @brief Copy constructor */
    inline SharedPtr(const SharedPtr &other) noexcept : _ptr(other._ptr) { if (_ptr) ++_ptr->count; }

    /** @brief Move constructor */
    inline SharedPtr(SharedPtr &&other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }


    /** @brief Copy assignment */
    inline SharedPtr &operator=(const SharedPtr &other) noexcept
    {
        release<false>();
        _ptr = other._ptr;
        if (_ptr)
            ++_ptr->count;
        return *this;
    }

    /** @brief Move assignment */
    inline SharedPtr &operator=(SharedPtr &&other) noexcept { swap(other); return *this; }


    /** @brief Boolean check operator */
    [[nodiscard]] explicit inline operator bool(void) const noexcept { return _ptr != nullptr; }

    /** @brief Boolean check operator */
    [[nodiscard]] explicit inline operator Type *(void) noexcept { return get(); }
    [[nodiscard]] explicit inline operator const Type *(void) const noexcept { return get(); }


    /** @brief Load atomic reference count */
    [[nodiscard]] inline std::uint32_t referenceCount(void) const noexcept { return _ptr ? _ptr->count.load() : 0u; }


    /** @brief Mutable managed object getter */
    [[nodiscard]] inline Type *get(void) noexcept { return &_ptr->value; }

    /** @brief Constant managed object getter */
    [[nodiscard]] inline const Type *get(void) const noexcept { return &_ptr->value; }


    /** @brief Mutable managed object pointer access */
    [[nodiscard]] inline Type *operator->(void) noexcept { return &_ptr->value; }

    /** @brief Constant managed object pointer access */
    [[nodiscard]] inline const Type *operator->(void) const noexcept { return &_ptr->value; }


    /** @brief Mutable managed object reference access */
    [[nodiscard]] inline Type &operator*(void) noexcept { return _ptr->value; }

    /** @brief Constant managed object reference access */
    [[nodiscard]] inline const Type &operator*(void) const noexcept { return _ptr->value; }


    /** @brief Swaps the managed objects */
    inline void swap(SharedPtr &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Destroy the managed object */
    template<bool ResetMembers = true>
    inline void release(void) noexcept { if (_ptr) releaseUnsafe<ResetMembers>(); }

    /** @brief Comparison operators */
    [[nodiscard]] inline bool operator==(const SharedPtr &other) const noexcept { return _ptr == other._ptr; }
    [[nodiscard]] inline bool operator!=(const SharedPtr &other) const noexcept { return _ptr != other._ptr; }
    [[nodiscard]] inline bool operator==(const Type * const other) const noexcept { return get() == other; }
    [[nodiscard]] inline bool operator!=(const Type * const other) const noexcept { return get() != other; }
    [[nodiscard]] inline bool operator==(Type * const other) const noexcept { return get() == other; }
    [[nodiscard]] inline bool operator!=(Type * const other) const noexcept { return get() != other; }

private:
    /** @brief Instance constructor */
    inline SharedPtr(Data * const ptr) : _ptr(ptr) {}

    /** @brief Unsafe implementation of the release function */
    template<bool ResetMembers = true>
    inline void releaseUnsafe(void) noexcept
    {
        if (!--_ptr->count) {
            _ptr->~Data();
            Allocator::Deallocate(_ptr, sizeof(Data), alignof(Data));
        }
        if constexpr (ResetMembers) {
            _ptr = nullptr;
        }
    }

    Data *_ptr {};
};