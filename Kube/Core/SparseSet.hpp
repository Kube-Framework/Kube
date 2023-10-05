/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sparse set
 */

#pragma once

#include "UniquePtr.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t PageSize, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range, auto Initializer>
        requires (Initializer == nullptr || std::is_invocable_v<decltype(Initializer), Type *, Type *>)
    class SparseSet;
}

/** @brief The sparse index set is a container which provide O(1) look-up time at the cost of non-efficient memory consumption
 *  @note This implementation is not aware of which index is initialized the user must manage lifecycle **carefully**
 *  If you wish to ensure initialization of */
template<typename Type, std::size_t PageSize, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator, std::integral Range = std::uint32_t, auto Initializer = nullptr>
    requires (Initializer == nullptr || std::is_invocable_v<decltype(Initializer), Type *, Type *>)
class kF::Core::SparseSet
{
public:
    /** @brief True if initializer is not null */
    static constexpr bool HasInitializer = !std::is_same_v<decltype(Initializer), std::nullptr_t>;

    /** @brief Page containing values */
    struct alignas(alignof(Type)) Page
    {
        std::uint8_t data[PageSize * sizeof(Type)];
    };

    /** @brief Pointer over page */
    using PagePtr = UniquePtr<Page, Allocator>;

    /** @brief If true, the set can be cleared and released safely */
    static constexpr bool IsSafeToClear = std::is_trivially_destructible_v<Type>;


    /** @brief Get page index of element */
    [[nodiscard]] static inline Range GetPageIndex(const Range index) noexcept { return index / Range(PageSize); }

    /** @brief Get element index inside page */
    [[nodiscard]] static inline Range GetElementIndex(const Range index) noexcept { return index % Range(PageSize); }


    /** @brief Default destructor */
    inline ~SparseSet(void) noexcept = default;

    /** @brief Default constructor */
    inline SparseSet(void) noexcept = default;

    /** @brief Default move constructor */
    inline SparseSet(SparseSet &&other) noexcept = default;

    /** @brief Move assignment */
    inline SparseSet &operator=(SparseSet &&other) noexcept = default;

    /** @brief Swap two instances */
    inline void swap(SparseSet &other) noexcept { _pages.swap(other._pages); }


    /** @brief Check if the page of an index exists */
    [[nodiscard]] inline bool pageExists(const Range index) const noexcept { return _pages.size() > GetPageIndex(index); }


    /** @brief Add a new value to the set */
    template<typename ...Args>
    Type &add(const Range index, Args &&...args) noexcept;


    /** @brief Remove a value from the set and return it */
    void remove(const Range index) noexcept
        { remove(GetPageIndex(index), GetElementIndex(index)); }

    /** @brief Remove a value by page & elemnt index */
    void remove(const Range pageIndex, const Range elementIndex) noexcept;


    /** @brief Extract and remove a value from the set and return it */
    [[nodiscard]] inline Type extract(const Range index) noexcept
        { return extract(GetPageIndex(index), GetElementIndex(index)); }

    /** @brief Extract and remove a value by page & elemnt index */
    [[nodiscard]] Type extract(const Range pageIndex, const Range elementIndex) noexcept;


    /** @brief Get value reference by index */
    [[nodiscard]] inline Type &at(const Range index) noexcept
        { return at(GetPageIndex(index), GetElementIndex(index)); }
    [[nodiscard]] inline const Type &at(const Range index) const noexcept
        { return at(GetPageIndex(index), GetElementIndex(index)); }

    /** @brief Get value reference by page & element index */
    [[nodiscard]] inline Type &at(const Range pageIndex, const Range elementIndex) noexcept
        { return reinterpret_cast<Type *>(_pages[pageIndex].get())[elementIndex]; }
    [[nodiscard]] inline const Type &at(const Range pageIndex, const Range elementIndex) const noexcept
        { return reinterpret_cast<const Type *>(_pages[pageIndex].get())[elementIndex]; }


    /** @brief Release all memory without calling any Type destructors */
    inline void clearUnsafe(void) noexcept { _pages.clear(); }

    /** @brief Release all memory without calling any Type destructors */
    inline void releaseUnsafe(void) noexcept { _pages.release(); }

private:
    Core::Vector<PagePtr, Allocator, Range> _pages {};
};

#include "SparseSet.ipp"