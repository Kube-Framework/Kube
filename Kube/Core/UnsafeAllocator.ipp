/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread unsafe allocator
 */

#include <memory>

#include "UnsafeAllocator.hpp"


template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::~UnsafeAllocator(void) noexcept
{
    if (_stack) {
        _stack->next = _busyStack;
        _busyStack = _stack;
    }
    AllocatorUtils::Internal::DestroyUnsafeAllocator(_pageSize, _busyStack);
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::UnsafeAllocator(void) noexcept
    : _pageSize(Platform::GetPageSize())
{
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void *kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocate(const std::size_t size, const std::size_t alignment) noexcept
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
inline void kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::deallocate(
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
inline void *kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocateFromBucket(
        const std::size_t bucketIndex) noexcept
{
    void *data = nullptr;

    // Try perfect bucket fit if possible
    if (auto &bucket = _buckets[bucketIndex]; bucket) [[likely]] {
        data = reinterpret_cast<void *>(bucket);
        bucket = bucket->next;
    // Perfect fit failed
    } else [[unlikely]] {
        data = allocateFromStack(static_cast<std::size_t>(1u) << (bucketIndex + MinSizePower));
    }
    return data;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::deallocateFromBucket(
        void * const data, const std::size_t bucketIndex) noexcept
{
    auto &bucket = _buckets[bucketIndex];
    const auto header = reinterpret_cast<AllocatorUtils::AllocationHeader *>(data);
    header->next = bucket;
    bucket = header;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void *kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::allocateFromStack(
    const std::size_t bucketSize) noexcept
{
    void *data {};

    while (true) {
        // Check for an existing stack
        if (_stack) [[likely]] {
            const auto availableSize = _tail - _head;
            auto space = availableSize;
            auto stackPtr = _stack->dataAt(_head);

            // If the stack can allocate the required memory, reserve it
            if (std::align(bucketSize, bucketSize, stackPtr, space) != nullptr) [[likely]] {
                data = stackPtr;
                // Try to fragment any padding introduced by alignment
                if (availableSize != space) {
                    // Stack block fragmentation decrease performances by a bit but reduce lost memory
                    fragmentStackBlock(availableSize - space);
                }
                _head += bucketSize;
                break;

            // Else the stack is insufficient to hold the allocation, fragment it and put it in busy list
            } else [[unlikely]] {
                fragmentStack();
            }

        // No stack are allocated, build a new one
        } else if (!buildStack(bucketSize)) [[unlikely]] {
            break;
        }
    }
    return data;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline bool kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::buildStack(
        const std::size_t bucketSize) noexcept
{
    auto stackSize = AllocatorUtils::GetStackSize<sizeof(AllocatorUtils::UnsafeStackMetaData), MaxStackSize>(
        bucketSize,
        _pageSize,
        _tail
    );
    const auto data = AllocatorUtils::FallbackAllocate(stackSize, _pageSize);

    if (data) [[likely]] {
        _stack = new (data) AllocatorUtils::UnsafeStackMetaData {
            .size = stackSize,
            .next = nullptr
        };
        _head = sizeof(AllocatorUtils::UnsafeStackMetaData);
        _tail = stackSize;
        return true;
    } else [[unlikely]]
        return false;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::fragmentStack(void) noexcept
{
    // Fragment all available stack size
    fragmentStackBlock(_tail - _head);

    // Insert the stack in busy list
    _stack->next = _busyStack;
    _busyStack = _stack;
    _stack = nullptr;
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline void kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::fragmentStackBlock(
        const std::size_t size) noexcept
{
    auto availableSize = size;
    auto head = _head;

    // Increment the real head in order to skip memory if not fragmentable
    _head += size;

    // Loop until the size left is inferior of minimum size of a bucket
    while (availableSize >= MinSize) {
        // Determine the block power and size that could fit available size
        const auto blockPower = AllocatorUtils::FindBucketFit<MaxSizePower>(availableSize, head);

        // If this block is retainable, insert it into a bucket
        if (blockPower >= MinSizePower) [[likely]] {
            const auto blockPtr = _stack->allocationAt(head);
            auto &bucket = _buckets[blockPower - MinSizePower];
            blockPtr->next = bucket;
            bucket = blockPtr;
        }

        // Reduce available size by fragmented block size
        const auto blockSize = static_cast<std::size_t>(1u) << blockPower;
        head += blockSize;
        availableSize -= blockSize;
    }
}

template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSizePower>
inline bool kF::Core::UnsafeAllocator<MinSizePower, MaxSizePower, MaxStackSizePower>::empty(void) noexcept
{
    for (const auto &bucket : _buckets) {
        if (bucket)
            return false;
    }
    return true;
}
