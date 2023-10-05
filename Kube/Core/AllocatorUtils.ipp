/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocator utilities
 */

#include "AllocatorUtils.hpp"

template<std::size_t MinSizePower>
inline std::size_t kF::Core::AllocatorUtils::GetBucketIndex(const std::size_t size) noexcept
{
    std::size_t bucketIndex { NextPowerOf2Bit(size) };

    if (bucketIndex >= MinSizePower) [[likely]]
        bucketIndex -= MinSizePower;
    else [[unlikely]]
        bucketIndex = 0;
    return bucketIndex;
}

template<std::size_t MaxSizePower>
inline std::size_t kF::Core::AllocatorUtils::FindBucketFit(const std::size_t availableSize, const std::size_t head) noexcept
{
    auto blockSize = static_cast<std::size_t>(1u) << MaxSizePower;
    auto blockPower = MaxSizePower;
    // Decrement block size until available size can fit into it
    while (availableSize < blockSize) {
        blockSize >>= 1;
        --blockPower;
    }

    // Continue decrementing block size as long as head is not aligned with block size
    while (head & (blockSize - 1)) {
        blockSize >>= 1;
        --blockPower;
    }
    return blockPower;
}

template<std::size_t StackMetaDataSize, std::size_t MaxStackSize>
inline std::size_t kF::Core::AllocatorUtils::GetStackSize(const std::size_t bucketSize, const std::size_t pageSize, const std::size_t lastStackSize) noexcept
{
    // 'bucketSize << 4u': Ensure we have at least 16 times the bucket size in the stack
    auto stackSize = bucketSize << 4u;
    if (stackSize <= lastStackSize) // Last stack size is either 0 or > pageSize
        stackSize = lastStackSize << static_cast<decltype(stackSize)>(stackSize == lastStackSize);
    else if (stackSize < pageSize)
        stackSize = pageSize;
    return std::min(stackSize, MaxStackSize);
}