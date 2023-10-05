/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#pragma once

#include <atomic>
#include <cstdlib>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
    class MPMCQueue;
}

/**
 * @brief The MPMC queue is a lock-free queue that supports Multiple Producers and Multiple Consumers
 *
 * @tparam Type to be inserted
 * @tparam Allocator Static allocator
 */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class alignas_double_cacheline kF::Core::MPMCQueue
{
public:
    /** @brief Each cell represent the queued type and a sequence index */
    struct Cell
    {
        std::atomic<std::size_t> sequence { 0 };
        Type data;
    };

    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        std::size_t mask { 0 };
        Cell *data {};
    };

    /** @brief Cache of producers or consumers */
    struct Cache
    {
        Buffer buffer {};
    };


    /** @brief Destruct and release all memory (unsafe) */
    ~MPMCQueue(void) noexcept;

    /** @brief Default constructor initialize the queue */
    MPMCQueue(const std::size_t capacity) noexcept;


    /** @brief Return the size of the queue */
    [[nodiscard]] std::size_t size(void) const noexcept;


    /** @brief Push a single element into the queue
     *  @tparam Set MoveOnSuccess to true to move 'args' instead of forward on push success
     *  @return true if the element has been inserted */
    template<bool MoveOnSuccess = false, typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept;


    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept;


    /** @brief Clear all elements of the queue (unsafe) */
    inline void clear(void) noexcept { for (Type tmp; pop(tmp);); }


private:
    alignas_cacheline std::atomic<std::size_t> _tail { 0 }; // Tail accessed by producers
    alignas_cacheline Cache _tailCache {}; // Cache accessed by producers
    alignas_cacheline std::atomic<std::size_t> _head { 0 }; // Head accessed by consumers
    alignas_cacheline Cache _headCache {}; // Cache accessed by consumers


    /** @brief Copy and move constructors disabled */
    MPMCQueue(const MPMCQueue &other) = delete;
    MPMCQueue(MPMCQueue &&other) = delete;
};

static_assert_sizeof(kF::Core::MPMCQueue<int>, 2 * kF::Core::CacheLineDoubleSize);
static_assert_alignof_double_cacheline(kF::Core::MPMCQueue<int>);

#include "MPMCQueue.ipp"