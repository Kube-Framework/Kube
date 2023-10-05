/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPMC Queue
 */

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::SPMCQueue<Type, Allocator>::~SPMCQueue(void) noexcept
{
    clear();
    Allocator::Deallocate(_headCache.buffer.data, sizeof(Type) * _headCache.buffer.capacity, alignof(Type));
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::SPMCQueue<Type, Allocator>::SPMCQueue(const std::size_t capacity, const bool usedAsBuffer) noexcept
{
    _tailCache.buffer.capacity = capacity + usedAsBuffer;
    _tailCache.buffer.data = reinterpret_cast<Type *>(Allocator::Allocate(sizeof(Type) * _tailCache.buffer.capacity, alignof(Type)));
    _headCache.buffer = _tailCache.buffer;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool MoveOnSuccess, typename ...Args>
inline bool kF::Core::SPMCQueue<Type, Allocator>::push(Args &&...args) noexcept
{
    const auto tail = _tail.load(std::memory_order_acquire);
    auto next = tail + 1;

    if (next == _tailCache.buffer.capacity) [[unlikely]]
        next = 0;
    if (auto head = _tailCache.value; next == head) [[unlikely]] {
        head = _tailCache.value = _head.load(std::memory_order_acquire);
        if (next == head) [[unlikely]]
            return false;
    }
    if constexpr (MoveOnSuccess)
        new (_tailCache.buffer.data + tail) Type(std::move(args)...);
    else
        new (_tailCache.buffer.data + tail) Type(std::forward<Args>(args)...);
    _tail.store(next, std::memory_order_release);
    return true;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline bool kF::Core::SPMCQueue<Type, Allocator>::pop(Type &value) noexcept
{
    auto head = _headCache.value.load(std::memory_order_acquire);
    auto tail = _tail.load(std::memory_order_acquire);
    decltype(head) next = 0;

    // If queue is empty abort
    if (head == tail) [[unlikely]]
        return false;
    while (true) {
        next = head + 1;
        if (next == _headCache.buffer.capacity) [[unlikely]]
            next = 0;
        // Try to set the head value shared to consumers to peserve data during extraction
        if (_headCache.value.compare_exchange_weak(head, next, std::memory_order_acq_rel)) [[likely]]
            break;
        // On failure, check the new head
        else if (head != tail) [[likely]] {
            continue;
        // If the new head is at tail, try to load tail again
        } else [[unlikely]] {
            tail = _tail.load(std::memory_order_acquire);
            if (head == tail) [[unlikely]]
                return false;
        }
    }
    // Transaction is secured until head is modified
    auto *elem = reinterpret_cast<Type *>(_headCache.buffer.data + head);
    if constexpr (std::is_move_assignable_v<Type>)
        value = std::move(*elem);
    else
        value = *elem;
    elem->~Type();
    // Loop while the transaction is not done (may wait prior thread with longer extraction)
    auto expected = head;
    while (!_head.compare_exchange_weak(expected, next, std::memory_order_acq_rel)) [[unlikely]]
        expected = head;
    return true;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool AllowLess, std::input_iterator InputIterator>
inline std::size_t kF::Core::SPMCQueue<Type, Allocator>::pushRangeImpl(const InputIterator from, const InputIterator to) noexcept
{
    auto toPush = static_cast<std::size_t>(to - from);
    const auto tail = _tail.load(std::memory_order_relaxed);
    const auto capacity = _tailCache.buffer.capacity;
    auto head = _tailCache.value;
    auto available = capacity - (tail - head);

    if (available > capacity) [[unlikely]]
        available -= capacity;
    if (toPush >= available) [[unlikely]] {
        head = _tailCache.value = _head.load(std::memory_order_acquire);
        available = capacity - (tail - head);
        if (available > capacity) [[unlikely]]
            available -= capacity;
        if (toPush >= available) [[unlikely]] {
            if constexpr (AllowLess)
                toPush = available - 1;
            else
                return 0;
        }
    }
    auto next = tail + toPush;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        const auto split = toPush - next;
        std::uninitialized_move_n(from, split, _tailCache.buffer.data + tail);
        std::destroy_n(from, split);
        std::uninitialized_move_n(from + split, next, _tailCache.buffer.data);
        std::destroy_n(from + split, next);
    } else {
        std::uninitialized_move_n(from, toPush, _tailCache.buffer.data + tail);
        std::destroy_n(from, toPush);
    }
    _tail.store(next, std::memory_order_release);
    return toPush;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool AllowLess, typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
inline std::size_t kF::Core::SPMCQueue<Type, Allocator>::popRangeImpl(const OutputIterator from, const OutputIterator to) noexcept
{
    const auto capacity = _headCache.buffer.capacity;
    auto head = _headCache.value.load(std::memory_order_acquire);
    auto tail = _tail.load(std::memory_order_acquire);
    std::size_t toPop = to - from;
    decltype(head) next = 0;
    bool splitExtract = false;

    // If queue is empty abort
    if (head == tail) [[unlikely]]
        return false;
    while (true) {
        // Determine the pop count
        auto available = tail - head;
        if (available > capacity) [[unlikely]]
            available += capacity;
        if (toPop >= available) [[unlikely]] {
            tail = _tail.load(std::memory_order_acquire);
            available = tail - head;
            if (available > capacity) [[unlikely]]
                available += capacity;
            if (toPop > available) [[unlikely]] {
                if constexpr (AllowLess)
                    toPop = available;
                else
                    return 0;
            }
        }
        // Determine the next head
        next = head + toPop;
        if (next >= capacity) [[unlikely]] {
            next -= capacity;
            splitExtract = true;
        } else
            splitExtract = false;
        // Try to set the head value shared for consumers to peserve data during extraction
        if (_headCache.value.compare_exchange_weak(head, next, std::memory_order_acq_rel)) [[likely]]
            break;
    }
    // Transaction is secured until head is modified
    if (splitExtract) [[unlikely]] {
        const auto split = toPop - next;
        std::copy_n(std::make_move_iterator(_headCache.buffer.data + head), split, from);
        std::destroy_n(_headCache.buffer.data + head, split);
        std::copy_n(std::make_move_iterator(_headCache.buffer.data), next, from + split);
        std::destroy_n(_headCache.buffer.data, next);
    } else {
        std::copy_n(std::make_move_iterator(_headCache.buffer.data + head), toPop, from);
        std::destroy_n(_headCache.buffer.data + head, toPop);
    }
    // Loop while the transaction is not done (may wait prior thread with longer extraction)
    auto expected = head;
    while (!_head.compare_exchange_weak(expected, next, std::memory_order_acq_rel)) [[unlikely]]
        expected = head;
    return toPop;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::Core::SPMCQueue<Type, Allocator>::clear(void) noexcept
{
    for (Type type; pop(type););
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline std::size_t kF::Core::SPMCQueue<Type, Allocator>::size(void) const noexcept
{
    const auto tail = _tail.load(std::memory_order_seq_cst);
    const auto capacity = _tailCache.buffer.capacity;
    const auto head = _head.load(std::memory_order_seq_cst);
    auto available = tail - head;
    if (available > capacity) [[unlikely]]
        available += capacity;
    return available;
}