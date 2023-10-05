/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVectorBase
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    struct NoCustomHeaderType {};

    /** @brief Deduce the alignment of FlatVectorHeader */
    template<typename Type, std::integral Range, std::size_t CustomHeaderTypeSize>
    [[nodiscard]] constexpr std::size_t GetFlatVectorHeaderAlignment(void)
    {
        constexpr std::size_t TotalHeaderSize = sizeof(Range) * 2 + CustomHeaderTypeSize;
        constexpr std::size_t MinAlignment = alignof(Type);

        if constexpr (TotalHeaderSize > MinAlignment)
            return kF::Core::NextPowerOf2(TotalHeaderSize);
        else
            return MinAlignment;
    }

    /** @brief Header of the FlatVector with custom type */
    template<typename Type, typename CustomHeaderType, std::integral Range, std::size_t CustomHeaderTypeSize = sizeof(CustomHeaderType)>
    struct alignas(GetFlatVectorHeaderAlignment<Type, Range, CustomHeaderTypeSize>()) FlatVectorHeader
    {
        CustomHeaderType customType {};
        Range size {};
        Range capacity {};
    };

    /** @brief Header of the FlatVector without custom type */
    template<typename Type, std::integral Range>
    struct alignas(GetFlatVectorHeaderAlignment<Type, Range, 0>()) FlatVectorHeader<Type, NoCustomHeaderType, Range>
    {
        Range size {};
        Range capacity {};
    };

    static_assert_fit_cacheline(TEMPLATE_TYPE(FlatVectorHeader, std::size_t, char[48], std::size_t));
    static_assert_fit_eighth_cacheline(TEMPLATE_TYPE(FlatVectorHeader, std::size_t, NoCustomHeaderType, std::uint32_t));

    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
    class FlatVectorBase;
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
class kF::Core::Internal::FlatVectorBase
{
public:
    /** @brief Output iterator */
    using Iterator = Type *;

    /** @brief Input iterator */
    using ConstIterator = const Type *;

    /** @brief FlatVector's header */
    using Header = FlatVectorHeader<Type, CustomHeaderType, Range>;


    /** @brief Check if the instance is safe to access */
    [[nodiscard]] inline bool isSafe(void) const noexcept { return _ptr; }


    /** @brief Fast empty check */
    [[nodiscard]] inline bool empty(void) const noexcept { return !_ptr || !sizeUnsafe(); }


    /** @brief Get internal data pointer */
    [[nodiscard]] inline Type *data(void) noexcept { return const_cast<Type *>(const_cast<const FlatVectorBase *>(this)->data()); }
    [[nodiscard]] const Type *data(void) const noexcept;
    [[nodiscard]] inline Type *dataUnsafe(void) noexcept { return reinterpret_cast<Type *>(_ptr + 1); }
    [[nodiscard]] inline const Type *dataUnsafe(void) const noexcept { return reinterpret_cast<const Type *>(_ptr + 1); }


    /** @brief Get the size of the vector */
    [[nodiscard]] inline Range size(void) const noexcept { return _ptr ? sizeUnsafe() : Range(); }
    [[nodiscard]] inline Range sizeUnsafe(void) const noexcept { return _ptr->size; }


    /** @brief Get the capacity of the vector */
    [[nodiscard]] inline Range capacity(void) const noexcept { return _ptr ? capacityUnsafe() : Range(); }
    [[nodiscard]] inline Range capacityUnsafe(void) const noexcept { return _ptr->capacity; }


    /** @brief Begin / end overloads */
    [[nodiscard]] inline Iterator begin(void) noexcept { return _ptr ? beginUnsafe() : Iterator(); }
    [[nodiscard]] inline Iterator end(void) noexcept { return _ptr ? endUnsafe() : Iterator(); }
    [[nodiscard]] inline ConstIterator begin(void) const noexcept { return _ptr ? beginUnsafe() : ConstIterator(); }
    [[nodiscard]] inline ConstIterator end(void) const noexcept { return _ptr ? endUnsafe() : ConstIterator(); }


    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] inline Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] inline Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] inline ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] inline ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }


    /** @brief Steal another instance */
    void steal(FlatVectorBase &other) noexcept;

    /** @brief Swap two instances */
    void swap(FlatVectorBase &other) noexcept { std::swap(_ptr, other._ptr); }


    /** @brief Get the custom type in header if any (doesn't check if the vector is allocated) */
    template<typename As = CustomHeaderType>
    [[nodiscard]] inline std::enable_if_t<!std::is_same_v<As, NoCustomHeaderType>, As &> headerCustomType(void) noexcept
        { return _ptr->customType; }
    template<typename As = CustomHeaderType>
    [[nodiscard]] inline std::enable_if_t<!std::is_same_v<As, NoCustomHeaderType>, const As &> headerCustomType(void) const noexcept
        { return _ptr->customType; }

protected:
    Header *_ptr {};

    /** @brief Protected data setter */
    inline void setData(Type * const data) noexcept { _ptr = reinterpret_cast<Header *>(data) - 1; }

    /** @brief Protected size setter */
    inline void setSize(const Range size) noexcept { _ptr->size = size; }

    /** @brief Protected capacity setter */
    inline void setCapacity(const Range capacity) noexcept { _ptr->capacity = capacity; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept;
};

#include "FlatVectorBase.ipp"