/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#include "Abort.hpp"

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::MPMCQueue<Type, Allocator>::~MPMCQueue(void) noexcept
{
    clear();
    Allocator::Deallocate(_headCache.buffer.data, sizeof(Cell) * (_headCache.buffer.mask + 1), alignof(Cell));
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::Core::MPMCQueue<Type, Allocator>::MPMCQueue(const std::size_t capacity) noexcept
    : _tailCache(Cache { Buffer { capacity - 1, nullptr } })
{
    kFEnsure((capacity >= 2) && (capacity & (capacity - 1ul)) == 0, // Ensure capacity is  a power of two >= 2
        "Core::MPMCQueue: Buffer capacity must be a power of 2 (", capacity, ')');
    _tailCache.buffer.data = reinterpret_cast<Cell *>(Allocator::Allocate(sizeof(Cell) * capacity, alignof(Cell)));
    kFEnsure(_tailCache.buffer.data, // Ensure allocation succeed
        "Core::MPMCQueue: Allocation of capacity ", capacity, " failed");

    // Init cells
    for (auto i = 0ul; i != capacity; ++i)
        new (&_tailCache.buffer.data[i].sequence) decltype(Cell::sequence)(i);
    _headCache = _tailCache;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline std::size_t kF::Core::MPMCQueue<Type, Allocator>::size(void) const noexcept
{
    return _tail.load(std::memory_order_relaxed) - _head.load(std::memory_order_relaxed);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool MoveOnSuccess, typename ...Args>
inline bool kF::Core::MPMCQueue<Type, Allocator>::push(Args &&...args) noexcept
{
    auto pos = _tail.load(std::memory_order_relaxed);
    auto * const data = _tailCache.buffer.data;
    const auto mask = _tailCache.buffer.mask;
    Cell *cell;

    while (true) {
        cell = &data[pos & mask];
        const auto sequence = cell->sequence.load(std::memory_order_acquire);
        if (sequence == pos) [[likely]] {
            if (_tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) [[likely]]
                break;
        } else if (sequence < pos) [[unlikely]]
            return false;
        else
            pos = _tail.load(std::memory_order_relaxed);
    }
    if constexpr (MoveOnSuccess)
        new (&cell->data) Type(std::move(args)...);
    else
        new (&cell->data) Type(std::forward<Args>(args)...);
    cell->sequence.store(pos + 1, std::memory_order_release);
    return true;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline bool kF::Core::MPMCQueue<Type, Allocator>::pop(Type &value) noexcept
{
    auto pos = _head.load(std::memory_order_relaxed);
    const auto mask = _headCache.buffer.mask;
    auto * const data = _headCache.buffer.data;
    Cell *cell;

    while (true) {
        cell = &data[pos & mask];
        const auto sequence = cell->sequence.load(std::memory_order_acquire);
        const auto next = pos + 1;
        if (sequence == next) [[likely]] {
            if (_head.compare_exchange_weak(pos, next, std::memory_order_relaxed)) [[likely]]
                break;
        } else if (sequence < next) [[unlikely]]
            return false;
        else
            pos = _head.load(std::memory_order_relaxed);
    }
    if constexpr (std::is_move_assignable_v<Type>)
        value = std::move(cell->data);
    else
        value = cell->data;
    cell->data.~Type();
    cell->sequence.store(pos + mask + 1, std::memory_order_release);
    return true;
}
