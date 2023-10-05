/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVectorDetails
 */

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::push(Args &&...args) noexcept
{
    Type value(std::forward<Args>(args)...);
    const auto it = findSortedPlacement(value);
    return *DetailsBase::insert(it, { std::move(value) });
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::insertDefault(const Range count) noexcept
{
    return DetailsBase::insertDefault(findSortedPlacement(Type{}), count);
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::insertFill(
        const Range count, const Type &value) noexcept
{
    return DetailsBase::insertFill(findSortedPlacement(value), count, value);
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::insert(
        const InputIterator from, const InputIterator to) noexcept
{
    DetailsBase::insert(DetailsBase::end(), from, to);
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::insert(
        const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    DetailsBase::insert(DetailsBase::end(), from, to, std::forward<Map>(map));
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename InsertFunc>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::insertCustom(
        const Range count, InsertFunc &&insertFunc) noexcept
{
    DetailsBase::insertCustom(count, std::forward<InsertFunc>(insertFunc));
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(
        const InputIterator from, const InputIterator to) noexcept
{
    DetailsBase::resize(from, to);
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(
        const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    DetailsBase::resize(from, to, std::forward<Map>(map));
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename Initializer>
    requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const Range count, Initializer &&initializer) noexcept
{
    DetailsBase::resize(count, std::forward<Initializer>(initializer));
    sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename AssignType>
inline Range kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsRuntimeAllocated>::assign(const Range index, AssignType &&value) noexcept
{
    const auto count = DetailsBase::sizeUnsafe();
    auto &elem = DetailsBase::at(index);
    Range finalPos = index;

    elem = std::forward<AssignType>(value);
    if (index > 0 && !Compare{}(DetailsBase::at(index - 1), elem)) {
        finalPos = index - 1;
        for (auto i = index - 2; i > 0; --i) {
            if (!Compare{}(DetailsBase::at(i), elem))
                finalPos = i;
            else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    } else if (index + 1 < count && !Compare{}(elem, DetailsBase::at(index + 1))) {
        finalPos = index + 1;
        for (auto i = index + 2; i != count; ++i) {
            if (!Compare{}(elem, DetailsBase::at(i))) {
                finalPos = i;
            } else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    }
    return finalPos;
}