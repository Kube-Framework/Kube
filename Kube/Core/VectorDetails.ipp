/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::push(Args &&...args) noexcept
{
    if (!isSafe())
        reserveUnsafe<false>(static_cast<Range>(2));
    else if (sizeUnsafe() == capacityUnsafe())
        grow(static_cast<Range>(1));
    const Range currentSize = sizeUnsafe();
    Type * const elem = dataUnsafe() + currentSize;
    setSize(static_cast<Range>(currentSize + static_cast<Range>(1)));
    new (elem) Type(std::forward<Args>(args)...);
    return *elem;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::pop(void) noexcept
{
    const auto desiredSize = sizeUnsafe() - 1;

    dataUnsafe()[desiredSize].~Type();
    setSize(desiredSize);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::insertDefault(const Iterator pos, const Range count) noexcept
{
    return insertCustom(
        pos,
        count,
        [](const auto count, const auto out) {
            std::uninitialized_value_construct_n(out, count);
        }
    );
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::insertFill(
        const Iterator pos, const Range count, const Type &value) noexcept
{
    return insertCustom(
        pos,
        count,
        [&value](const auto count, const auto out) {
            std::fill_n(out, count, value);
        }
    );
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::insert(
        const Iterator pos, const InputIterator from, const InputIterator to) noexcept
{
    return insertCustom(
        pos,
        static_cast<Range>(std::distance(from, to)),
        [from](const auto count, const auto out) {
            std::uninitialized_copy_n(from, count, out);
        }
    );
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator, typename Map>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::insert(
        const Iterator pos, const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    return insertCustom(
        pos,
        static_cast<Range>(std::distance(from, to)),
        [from, map = std::forward<Map>(map)](const auto count, const auto out) {
            for (Range i {}; i != count; ++i) {
                if constexpr (IsMoveIterator<InputIterator>::Value) {
                    if constexpr (std::is_invocable_v<Map, decltype(std::move(from[i])), Range>)
                        new (out + i) Type(map(std::move(from[i]), i));
                    else
                        new (out + i) Type(map(std::move(from[i])));
                } else {
                    if constexpr (std::is_invocable_v<Map, decltype(from[i]), Range>)
                        new (out + i) Type(map(from[i], i));
                    else
                        new (out + i) Type(map(from[i]));
                }
            }
        }
    );
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename InsertFunc>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::insertCustom(
        const Iterator pos, const Range count, InsertFunc &&insertFunc) noexcept
{
    Range position;

    if (!count) [[unlikely]]
        return end();
    else if (pos == Iterator()) [[unlikely]] {
        reserve(count);
        position = 0;
    } else [[likely]]
        position = static_cast<Range>(std::distance(beginUnsafe(), pos));
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    if (const Range currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                insertFunc(count, tmpData + position);
                return tmpData + position;
            }
        }
        insertFunc(count, tmpData + position);
        if (position == currentSize) {
            std::uninitialized_move(currentBegin, currentEnd, tmpData);
        } else {
            const auto mid = currentBegin + position;
            std::uninitialized_move(currentBegin, mid, tmpData);
            std::uninitialized_move(mid, currentEnd, tmpData + position + count);
        }
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    insertFunc(count, currentBegin + position);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::erase(Iterator from, Iterator to) noexcept
{
    if (from == to) [[unlikely]]
        return to;
    const auto end = endUnsafe();
    setSize(sizeUnsafe() - static_cast<Range>(std::distance(from, to)));
    std::destroy(from, to);
    std::uninitialized_move(to, end, from);
    return from;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resizeUninitialized(const Range count) noexcept
{
    if (!isSafe()) {
        reserveUnsafe<false, false>(count);
    } else {
        clearUnsafe();
        reserveUnsafe<true, false>(count);
    }
    if (isSafe()) [[likely]]
        setSize(count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const Range count) noexcept
    requires std::constructible_from<Type>
{
    resizeUninitialized(count);
    if (count) [[likely]]
        std::uninitialized_value_construct_n(dataUnsafe(), count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const Range count, const Type &value) noexcept
    requires std::copy_constructible<Type>
{
    resizeUninitialized(count);
    if (count) [[likely]]
        std::uninitialized_fill_n(dataUnsafe(), count, value);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<typename Initializer>
    requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const Range count, Initializer &&initializer) noexcept
{
    resizeUninitialized(count);
    if (count) [[likely]] {
        auto out = beginUnsafe();
        for (Range i {}; i != count; ++i) {
            if constexpr (std::is_invocable_r_v<Type, Initializer, Range>)
                new (out + i) Type(initializer(i));
            else
                new (out + i) Type(initializer());
        }
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const InputIterator from, const InputIterator to) noexcept
{
    const auto count = static_cast<Range>(std::distance(from, to));

    resizeUninitialized(count);
    if (count) [[likely]]
        std::uninitialized_copy(from, to, beginUnsafe());
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::resize(const InputIterator from, const InputIterator to, Map &&map) noexcept
{
    const auto count = static_cast<Range>(std::distance(from, to));

    resizeUninitialized(count);
    if (count) [[likely]] {
        auto out = beginUnsafe();
        for (Range i {}; i != count; ++i) {
            if constexpr (IsMoveIterator<InputIterator>::Value) {
                if constexpr (std::is_invocable_v<Map, decltype(std::move(from[i])), Range>)
                    new (out + i) Type(map(std::move(from[i]), i));
                else
                    new (out + i) Type(map(std::move(from[i])));
            } else {
                if constexpr (std::is_invocable_v<Map, decltype(from[i]), Range>)
                    new (out + i) Type(map(from[i], i));
                else
                    new (out + i) Type(map(from[i]));
            }
        }
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::clear(void) noexcept
{
    if (isSafe()) [[likely]]
        clearUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::clearUnsafe(void) noexcept
{
    std::destroy_n(dataUnsafe(), sizeUnsafe());
    setSize(0);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::release(void) noexcept
{
    if (isSafe()) [[likely]]
        releaseUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::releaseUnsafe(void) noexcept
{
    const auto currentData = dataUnsafe();
    const Range currentCapacity = capacityUnsafe();

    clearUnsafe();
    setCapacity(0);
    setData(nullptr);
    deallocate(currentData, currentCapacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::reserve(const Range capacity) noexcept
{
    if (isSafe())
        reserveUnsafe<true>(capacity);
    else
        reserveUnsafe<false>(capacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
template<bool IsSafe, bool IsPreserved>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::reserveUnsafe(const Range capacity) noexcept
{
    if constexpr (IsSafe) {
        const Range currentCapacity = capacityUnsafe();
        if (currentCapacity >= capacity) [[unlikely]]
            return;
        const auto currentSize = sizeUnsafe();
        const auto currentData = dataUnsafe();
        const auto tmpData = allocate(capacity);
        setData(tmpData);
        setSize(currentSize);
        setCapacity(capacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData)
                return;
        }
        if constexpr (IsPreserved) {
            std::uninitialized_move_n(currentData, currentSize, tmpData);
        }
        std::destroy_n(currentData, currentSize);
        deallocate(currentData, currentCapacity);
    } else {
        if (capacity == 0)
            return;
        setData(allocate(capacity));
        setSize(0);
        setCapacity(capacity);
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::grow(const Range minimum) noexcept
{
    const auto currentData = dataUnsafe();
    const Range currentSize = sizeUnsafe();
    const Range currentCapacity = capacityUnsafe();
    const Range desiredCapacity = static_cast<Range>(currentCapacity + static_cast<Range>(std::max(currentCapacity, minimum)));
    const auto tmpData = allocate(desiredCapacity);

    setData(tmpData);
    setSize(currentSize);
    setCapacity(desiredCapacity);
    if constexpr (IsSmallOptimized) {
        if (tmpData == currentData)
            return;
    }
    std::uninitialized_move_n(currentData, currentSize, tmpData);
    std::destroy_n(currentData, currentSize);
    deallocate(currentData, currentCapacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::move(const Range from_, const Range to_, const Range output_) noexcept
{
    kFAssert(output_ < from_ || output_ >= to_,
        "Core::VectorDetails::move: Invalid move range [", from_, ", ", to_, "[ -> ", output_);

    auto from = from_;
    auto to = to_;
    auto output = output_;
    if (output < from) {
        const auto tmp = from;
        from = output;
        output = to;
        to = tmp;
    } else if (output)
        ++output;
    const auto it = beginUnsafe();
    std::rotate(it + from, it + to, it + output);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsRuntimeAllocated>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsRuntimeAllocated>::operator==(const VectorDetails &other) const noexcept
    requires std::equality_comparable<Type>
{
    const auto count = size();
    const auto otherCount = other.size();

    if (count == otherCount) [[unlikely]] {
        if (count) [[likely]]
            return std::equal(beginUnsafe(), endUnsafe(), other.beginUnsafe());
        else [[unlikely]]
            return true;
    } else [[likely]]
        return false;
}
