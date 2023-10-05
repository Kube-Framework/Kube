/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPSC Queue
 */

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::MPSCQueue<Type, Allocator>::~MPSCQueue(void) noexcept
{
    clear();
    Allocator::Deallocate(_headCache.buffer.data, sizeof(Type) * _headCache.buffer.capacity, alignof(Type));
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::MPSCQueue<Type, Allocator>::MPSCQueue(const std::size_t capacity, const bool usedAsBuffer) noexcept
{
    _tailCache.buffer.capacity = capacity + usedAsBuffer;
    _tailCache.buffer.data = reinterpret_cast<Type *>(Allocator::Allocate(sizeof(Type) * _tailCache.buffer.capacity, alignof(Type)));
    _headCache.buffer = _tailCache.buffer;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool MoveOnSuccess, typename ...Args>
inline bool kF::Core::MPSCQueue<Type, Allocator>::push(Args &&...args) noexcept
{
    auto tail = _tailCache.value.load(std::memory_order_acquire);
    auto head = _head.load(std::memory_order_acquire);
    decltype(tail) next = 0;

    while (true) {
        next = tail + 1;
        if (next == _tailCache.buffer.capacity) [[unlikely]]
            next = 0;
        // If queue is full abort
        if (next == head) [[unlikely]]
            return false;
        // Try to set the tail value shared to producers to peserve data during insertion
        else if (_tailCache.value.compare_exchange_weak(tail, next, std::memory_order_acq_rel)) [[likely]]
            break;
    }
    // Transaction is secured until tail is modified
    if constexpr (MoveOnSuccess)
        new (_tailCache.buffer.data + tail) Type(std::move(args)...);
    else
        new (_tailCache.buffer.data + tail) Type(std::forward<Args>(args)...);
    // Loop while the transaction is not done (may wait prior thread with longer insertion)
    auto expected = tail;
    while (!_tail.compare_exchange_weak(expected, next, std::memory_order_acq_rel)) [[unlikely]]
        expected = tail;
    return true;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline bool kF::Core::MPSCQueue<Type, Allocator>::pop(Type &value) noexcept
{
    const auto head = _head.load(std::memory_order_relaxed);

    if (auto tail = _headCache.value; head == tail) [[unlikely]] {
        tail = _headCache.value = _tail.load(std::memory_order_acquire);
        if (head == tail) [[unlikely]]
            return false;
    }
    auto *elem = reinterpret_cast<Type *>(_headCache.buffer.data + head);
    auto next = head + 1;
    if (next == _headCache.buffer.capacity) [[unlikely]]
        next = 0;
    if constexpr (std::is_move_assignable_v<Type>)
        value = std::move(*elem);
    else
        value = *elem;
    elem->~Type();
    _head.store(next, std::memory_order_release);
    return true;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool AllowLess, std::input_iterator InputIterator>
inline std::size_t kF::Core::MPSCQueue<Type, Allocator>::pushRangeImpl(const InputIterator from, const InputIterator to) noexcept
{
    const auto capacity = _tailCache.buffer.capacity;
    auto tail = _tailCache.value.load(std::memory_order_acquire);
    auto head = _head.load(std::memory_order_acquire);
    std::size_t toPush = to - from;
    decltype(tail) next = 0;
    bool splitInsert = false;

    while (true) {
        auto available = tail - head;
        if (available > capacity) [[unlikely]]
            available -= capacity;
        if (toPush >= available) [[unlikely]] {
            head = _head.load(std::memory_order_acquire);
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
        // Determine the next tail
        next = tail + toPush;
        if (next >= capacity) [[unlikely]] {
            next -= capacity;
            splitInsert = true;
        } else
            splitInsert = false;
        // Try to set the tail value shared for producers to peserve data during extraction
        if (_tailCache.value.compare_exchange_weak(tail, next, std::memory_order_acq_rel)) [[likely]]
            break;
    }
    // Transaction is secured until tail is modified
    if (splitInsert) {
        const auto split = toPush - next;
        std::uninitialized_move_n(from, split, _tailCache.buffer.data + tail);
        std::destroy_n(from, split);
        std::uninitialized_move_n(from + split, next, _tailCache.buffer.data);
        std::destroy_n(from + split, next);
    } else {
        std::uninitialized_move_n(from, toPush, _tailCache.buffer.data + tail);
        std::destroy_n(from, toPush);
    }
    // Loop while the transaction is not done (may wait prior thread with longer insertion)
    auto expected = tail;
    while (!_tail.compare_exchange_weak(expected, next, std::memory_order_acq_rel)) [[unlikely]]
        expected = tail;
    return toPush;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool AllowLess, typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
inline std::size_t kF::Core::MPSCQueue<Type, Allocator>::popRangeImpl(const OutputIterator from, const OutputIterator to) noexcept
{
    std::size_t toPop = to - from;
    const auto head = _head.load(std::memory_order_relaxed);
    const auto capacity = _headCache.buffer.capacity;
    auto tail = _headCache.value;
    auto available = tail - head;

    if (available > capacity) [[unlikely]]
        available += capacity;
    if (toPop >= available) [[unlikely]] {
        tail = _headCache.value = _tail.load(std::memory_order_acquire);
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
    auto next = head + toPop;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        const auto split = toPop - next;
        std::copy_n(std::make_move_iterator(_headCache.buffer.data + head), split, from);
        std::destroy_n(_headCache.buffer.data + head, split);
        std::copy_n(std::make_move_iterator(_headCache.buffer.data), next, from + split);
        std::destroy_n(_headCache.buffer.data, next);
    } else {
        std::copy_n(std::make_move_iterator(_headCache.buffer.data + head), toPop, from);
        std::destroy_n(_headCache.buffer.data + head, toPop);
    }
    _head.store(next, std::memory_order_release);
    return toPop;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::Core::MPSCQueue<Type, Allocator>::clear(void) noexcept
{
    for (Type type; pop(type););
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline std::size_t kF::Core::MPSCQueue<Type, Allocator>::size(void) const noexcept
{
    const auto tail = _tail.load(std::memory_order_seq_cst);
    const auto capacity = _tailCache.buffer.capacity;
    const auto head = _head.load(std::memory_order_seq_cst);
    auto available = tail - head;
    if (available > capacity) [[unlikely]]
        available += capacity;
    return available;
}