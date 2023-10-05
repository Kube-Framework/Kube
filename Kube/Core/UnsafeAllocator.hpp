/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread unsafe allocator
 */

#pragma once

#include <array>

#include "IAllocator.hpp"
#include "AllocatorUtils.hpp"

namespace kF::Core
{
    template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSize>
    class UnsafeAllocator;

    namespace AllocatorUtils
    {
        /** @brief Meta data of a stack */
        struct alignas_quarter_cacheline UnsafeStackMetaData
        {
            std::size_t size { 0u };
            UnsafeStackMetaData *next {};

            /** @brief Get the stack data pointer at given byte index */
            [[nodiscard]] inline void *dataAt(const std::size_t at) noexcept
                { return reinterpret_cast<void *>(reinterpret_cast<std::uint8_t *>(this) + at); }

            /** @brief Get the stack data pointer at given byte index casted to allocation header */
            [[nodiscard]] inline AllocatorUtils::AllocationHeader *allocationAt(const std::size_t at) noexcept
                { return reinterpret_cast<AllocatorUtils::AllocationHeader *>(dataAt(at)); }
        };

        namespace Internal
        {
            /** @brief Non-template utility that destroys unsafe allocator stacks */
            void DestroyUnsafeAllocator(const std::size_t pageSize, UnsafeStackMetaData * const stack) noexcept;
        }
    }
}

/** @brief Unsynchronized memory allocator, used for general purposes allocations.
 *  Strength:   + Memory is split into aligned chunks that are reusable in many different size / alignment scenarios
 *  Weakness:   - No defragmentation: you may need to tune the template parameters to fit the scenario you are dealing with.
 *
 *  NB: By default, bucket fragmentation is disabled to fit the most general cases.
 *
 *  The implementation aims to bring efficiency of a stack based allocator with buckets to retain memory.
 *  This implementation ignores defragmentation and thus, you may need to tune the MinSizePower template parameter
 *  in order to not split too much the memory at the cost of small unused memory holes.
 *  Each bucket hold a memory allocation which is the size of a power of 2 and aligned over this size:
 *  32 bytes aligned over 32, 64 bytes aligned over 64, etc ...
 *  This allow fast fragmentation of the memory at the cost of defragmentation that becomes too heavy to be supported.
 *
 *
 *  @tparam MinSizePower The minimal allocation size a bucket can store
 *  @tparam MaxSizePower The maximal allocation size a bucket can store
 *  @tparam MaxStackSizePower The maximal allocation size a stack can have
 *
 *  @todo Benchmark an allocate implementation that prioritize stack allocation rather than fragmentation in case of non perfect fit
 *
 *  @note 1 << 16 == MMAP_THRESHOLD
*/
template<std::size_t MinSizePower = 5, std::size_t MaxSizePower = 12, std::size_t MaxStackSizePower = 16>
class alignas_double_cacheline kF::Core::UnsafeAllocator : public IAllocator
{
public:
    /** @brief Minimum retained allocation size in byte */
    static constexpr std::size_t MinSize = 1ul << MinSizePower;

    /** @brief Maximum retained allocation size in byte */
    static constexpr std::size_t MaxSize = 1ul << MaxSizePower;

    /** @brief Number of bucket to retain */
    static constexpr std::size_t BucketCount = (MaxSizePower - MinSizePower) + 1;

    /** @brief Maximum stack allocation size in byte */
    static constexpr std::size_t MaxStackSize = 1ul << MaxStackSizePower;


    static_assert(MaxStackSize > MaxSize);
    static_assert(BucketCount > 0, "BucketCount must be superior to 0");
    static_assert(MinSize >= sizeof(void *), "MinSize must be superior or equal to sizeof(void *)");


    /** @brief Virtual destructor */
    ~UnsafeAllocator(void) noexcept override;

    /** @brief Constructor */
    UnsafeAllocator(void) noexcept;

    /** @brief Disable copy constructor */
    UnsafeAllocator(const UnsafeAllocator &) noexcept = delete;

    /** @brief Disable copy assignment */
    UnsafeAllocator &operator=(const UnsafeAllocator &) noexcept = delete;


    /** @brief Allocate function implementation */
    [[nodiscard]] void *allocate(const std::size_t size, const std::size_t alignment) noexcept override;

    /** @brief Deallocate function implementation */
    void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept override;


    /** @brief Check if the allocator still has allocations
     *  @note This function is slow */
    [[nodiscard]] bool empty(void) noexcept;

private:
    /** @brief Allocate data from a specific bucket */
    [[nodiscard]] void *allocateFromBucket(const std::size_t bucketIndex) noexcept;

    /** @brief Deallocate data from a specific bucket */
    void deallocateFromBucket(void * const data, const std::size_t bucketIndex) noexcept;


    /** @brief Allocate a chunk from stack */
    [[nodiscard]] void *allocateFromStack(const std::size_t bucketSize) noexcept;


    /** @brief Build a new stack for internal allocation, considering the size of queried bucket */
    [[nodiscard]] bool buildStack(const std::size_t bucketSize) noexcept;


    /** @brief Fragment stack until end */
    void fragmentStack(void) noexcept;

    /** @brief Fragment a single block of the stack */
    void fragmentStackBlock(const std::size_t size) noexcept;


    const std::size_t _pageSize;
    std::size_t _allocationCount {};
    std::size_t _head { 0u };
    std::size_t _tail { 0u };
    AllocatorUtils::UnsafeStackMetaData *_stack {}; // Only 1 stack is used at a time
    AllocatorUtils::UnsafeStackMetaData *_busyStack {};
    std::array<AllocatorUtils::AllocationHeader *, BucketCount> _buckets {};
};

#include "UnsafeAllocator.ipp"