/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sparse set
 */

#include "Assert.hpp"
#include "SparseSet.hpp"

template<typename Type, std::size_t PageSize, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range, auto Initializer>
    requires (Initializer == nullptr || std::is_invocable_v<decltype(Initializer), Type *, Type *>)
template<typename ...Args>
inline Type &kF::Core::SparseSet<Type, PageSize, Allocator, Range, Initializer>::add(const Range index, Args &&...args) noexcept
{
    const auto pageIndex = GetPageIndex(index);
    const auto elementIndex = GetElementIndex(index);
    if (const auto size = _pages.size(); pageIndex >= size) [[unlikely]]
        _pages.insertDefault(_pages.end(), 1 + pageIndex - size);

    auto &page = _pages[pageIndex];
    if (!page) [[unlikely]] {
        page = PagePtr::Make();
        if constexpr (HasInitializer)
            Initializer(reinterpret_cast<Type *>(page.get()), reinterpret_cast<Type *>(page.get()) + PageSize);
    }

    return *new (&at(pageIndex, elementIndex)) Type(std::forward<Args>(args)...);
}

template<typename Type, std::size_t PageSize, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range, auto Initializer>
    requires (Initializer == nullptr || std::is_invocable_v<decltype(Initializer), Type *, Type *>)
inline void kF::Core::SparseSet<Type, PageSize, Allocator, Range, Initializer>::remove(const Range pageIndex, const Range elementIndex) noexcept
{
    auto &ref = at(pageIndex, elementIndex);
    if constexpr (!std::is_trivially_destructible_v<Type>)
        ref.~Type();
    if constexpr (HasInitializer)
        Initializer(&ref, &ref + 1);
}

template<typename Type, std::size_t PageSize, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range, auto Initializer>
    requires (Initializer == nullptr || std::is_invocable_v<decltype(Initializer), Type *, Type *>)
inline Type kF::Core::SparseSet<Type, PageSize, Allocator, Range, Initializer>::extract(const Range pageIndex, const Range elementIndex) noexcept
{
    auto &ref = at(pageIndex, elementIndex);
    Type value(std::move(ref));

    if constexpr (!std::is_trivially_destructible_v<Type>)
        ref.~Type();
    if constexpr (HasInitializer)
        Initializer(&ref, &ref + 1);
    return value;
}