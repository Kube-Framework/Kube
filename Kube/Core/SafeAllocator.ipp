/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread safe allocator
 */

#include <memory>

#include "SafeAllocator.hpp"

inline kF::Core::AllocatorUtils::SafeStackMetaData *kF::Core::AllocatorUtils::TryStealAtomicStack(AtomicStack &target) noexcept
{
    // Load the target allocation
    auto allocation = target.load(std::memory_order_acquire);

    // Try to steal an allocation as long as it still contains valid data
    while (true) {
        if (!allocation) [[unlikely]]
            break;
        decltype(allocation) next(allocation->next, allocation.tag() + 1);
        if (target.compare_exchange_weak(allocation, next, std::memory_order_acq_rel))
            break;
    }
    return allocation.get();
}

inline void kF::Core::AllocatorUtils::InsertAtomicStack(AtomicStack &target, SafeStackMetaData *stack) noexcept
{
    auto allocation = target.load(std::memory_order_acquire);

    while (true) {
        stack->next = allocation.get();
        decltype(allocation) next(stack, allocation.tag() + 1);
        if (target.compare_exchange_weak(allocation, next, std::memory_order_acq_rel))
            break;
    }
}

template<std::size_t Alignment>
inline void *kF::Core::AllocatorUtils::TryStealAtomicBucket(AtomicBucket<Alignment> &bucket) noexcept
{
    // Load the target allocation
    auto allocation = bucket.load(std::memory_order_acquire);

    // Try to steal an allocation as long as it still contains valid data
    while (true) {
        if (!allocation) [[unlikely]]
            break;
        decltype(allocation) next(allocation->next, allocation.tag() + 1);
        if (bucket.compare_exchange_weak(allocation, next, std::memory_order_acq_rel))
            break;
    }
    return allocation.get();
}

template<std::size_t Alignment>
inline void kF::Core::AllocatorUtils::InsertAtomicBucket(AtomicBucket<Alignment> &bucket, void * const toInsert) noexcept
{
    auto ptr = reinterpret_cast<AllocationHeader *>(toInsert);
    auto allocation = bucket.load(std::memory_order_acquire);

    while (true) {
        ptr->next = allocation.get();
        decltype(allocation) next(ptr, allocation.tag() + 1);
        if (bucket.compare_exchange_weak(allocation, next, std::memory_order_acq_rel))
            break;
    }
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::~SafeAllocator(void) noexcept
{
    while (true) {
        auto stack = AllocatorUtils::TryStealAtomicStack(_stack);
        if (stack)
            AllocatorUtils::InsertAtomicStack(_busyStack, stack);
        else
            break;
    }

    AllocatorUtils::DestroySafeAllocator(_pageSize, _busyStack.load().get());
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::SafeAllocator(void) noexcept
    : _pageSize(Platform::GetPageSize())
{
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void *kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocate(const std::size_t size, const std::size_t alignment) noexcept
{
    void *data = nullptr;

    // If the size fits into the maximum size a bucket can hold, look for existing buckets
    if (const auto targetSize = std::max(size, alignment); targetSize <= MaxSize) [[likely]] {
        // Find perfect bucket fit index
        data = allocateFromBucket(AllocatorUtils::GetBucketIndex<MinSizePower>(targetSize));
    // The required size is out of buckets retention range
    } else [[unlikely]] {
        data = AllocatorUtils::FallbackAllocate(size, alignment);
    }

    return data;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::deallocate(
        void * const data, const std::size_t size, const std::size_t alignment) noexcept
{
    auto targetSize = std::max(size, alignment);
    // If the size is retainable, insert it into a bucket
    if (data && targetSize <= MaxSize) [[likely]] {
        deallocateFromBucket(data, AllocatorUtils::GetBucketIndex<MinSizePower>(targetSize));
    // Else deallocate it
    } else [[unlikely]] {
        AllocatorUtils::FallbackDeallocate(data, size, alignment);
    }
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void *kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocateFromBucket(
        const std::size_t bucketIndex) noexcept
{
    void *data = nullptr;

    // Try perfect bucket fit if possible
    data = AllocatorUtils::TryStealAtomicBucket(_buckets[bucketIndex].value);
    // Else, allocate from a stack
    if (!data) [[unlikely]]
        data = allocateFromStack(static_cast<std::size_t>(1u) << (bucketIndex + MinSizePower));
    return data;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::deallocateFromBucket(
        void * const data, const std::size_t bucketIndex) noexcept
{
    AllocatorUtils::InsertAtomicBucket(_buckets[bucketIndex].value, data);
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void *kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocateFromStack(
    const std::size_t bucketSize) noexcept
{
    void *data {};

    // Steal a stack
    auto stack = AllocatorUtils::TryStealAtomicStack(_stack);

    while (true) {
        // Check for an existing stack
        if (stack) [[likely]] {
            const auto availableSize = stack->size - stack->head;
            auto space = availableSize;
            auto stackPtr = stack->dataHead();

            // If the stack can allocate the required memory, reserve it
            if (std::align(bucketSize, bucketSize, stackPtr, space) != nullptr) [[likely]] {
                data = stackPtr;
                // Try to fragment any padding introduced by alignment
                if (availableSize != space) {
                    // Stack block fragmentation decrease performances by a bit but reduce lost memory
                    fragmentStackBlock(stack, availableSize - space);
                }
                stack->head += bucketSize;
                break;

            // Else the stack is insufficient to hold the allocation, fragment it and put it in busy list
            } else [[unlikely]] {
                fragmentStack(stack);
                stack = nullptr;
            }

        // No stack are allocated, build a new one
        } else if (stack = buildStack(bucketSize); !stack) [[unlikely]] {
            break;
        }
    }
    // Insert the stack in active list
    if (stack->head != stack->size)
        AllocatorUtils::InsertAtomicStack(_stack, stack);
    else
        AllocatorUtils::InsertAtomicStack(_busyStack, stack);
    return data;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline kF::Core::AllocatorUtils::SafeStackMetaData *kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::buildStack(
        const std::size_t bucketSize) noexcept
{
    auto maxStackSize = _maxStackSize.load(std::memory_order_acquire);
    auto stackSize = AllocatorUtils::GetStackSize<sizeof(AllocatorUtils::SafeStackMetaData), MaxStackSize>(
        bucketSize,
        _pageSize,
        maxStackSize
    );
    const auto data = AllocatorUtils::FallbackAllocate(stackSize, _pageSize);
    AllocatorUtils::SafeStackMetaData *stack {};
    if (data) [[likely]] {
        stack = new (data) AllocatorUtils::SafeStackMetaData {
            .size = stackSize,
            .head = sizeof(AllocatorUtils::SafeStackMetaData),
            .next = nullptr
        };
    }

    // Set max stack size if higher than previous
    while (maxStackSize < stackSize && !_maxStackSize.compare_exchange_weak(maxStackSize, stackSize, std::memory_order_acq_rel));

    return stack;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::fragmentStack(
        AllocatorUtils::SafeStackMetaData * const stack) noexcept
{
    // Fragment all available stack size
    fragmentStackBlock(stack, stack->size - stack->head);

    // Insert the stack in busy list
    AllocatorUtils::InsertAtomicStack(_busyStack, stack);
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::fragmentStackBlock(
        AllocatorUtils::SafeStackMetaData * const stack, const std::size_t size) noexcept
{
    auto availableSize = size;
    auto head = stack->head;

    // Increment the real head in order to skip memory if not fragmentable
    stack->head += size;

    // Loop until the size left is inferior of minimum size of a bucket
    while (availableSize >= MinSize) {
        // Determine the block power and size that could fit available size
        const auto blockPower = AllocatorUtils::FindBucketFit<MaxSizePower>(availableSize, head);

        // If this block is retainable, insert it into a bucket
        if (blockPower >= MinSizePower) [[likely]]
            AllocatorUtils::InsertAtomicBucket(_buckets[blockPower - MinSizePower].value, stack->dataAt(head));

        // Reduce available size by fragmented block size
        const auto blockSize = static_cast<std::size_t>(1u) << blockPower;
        head += blockSize;
        availableSize -= blockSize;
    }
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline bool kF::Core::SafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::empty(void) noexcept
{
    for (const auto &bucket : _buckets) {
        if (bucket.value.load(std::memory_order_acquire))
            return false;
    }
    return true;
}
