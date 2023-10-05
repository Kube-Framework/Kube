/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

#pragma once

#include <atomic>
#include <cstdlib>
#include <memory>
#include <algorithm>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
    class SPSCQueue;
}

/**
 * @brief The SPSC queue is a lock-free queue that only supports a Single Producer and a Single Consumer
 * The queue is really fast compared to other more flexible implementations because the fact that only two thread can simultaneously read / write
 * means that less synchronization is needed for each operation.
 * The queue supports ranged push / pop to insert multiple elements without performance impact
 *
 * @tparam Type to be inserted
 */
template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class alignas_double_cacheline kF::Core::SPSCQueue
{
public:
    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        Type *data {};
        std::size_t capacity { 0 };
    };

    /** @brief Local thread cache */
    struct Cache
    {
        Buffer buffer {};
        std::size_t value { 0 };
    };


    /** @brief Destruct and release all memory (unsafe) */
    ~SPSCQueue(void) noexcept;

    /** @brief Default constructor initialize the queue
     * If 'usedAsBuffer' is true, capacity will be increased by 1 because the queue implementation needs one unused value when the queue is full */
    SPSCQueue(const std::size_t capacity, const bool usedAsBuffer = true) noexcept;


    /** @brief Push a single element into the queue
     *  @tparam Set MoveOnSuccess to true to move 'args' instead of forward on push success
     *  @return true if the element has been inserted */
    template<bool MoveOnSuccess = false, typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept;


    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept;


    /** @brief Push exactly 'count' elements into the queue
     *  @return Success on true */
    template<std::input_iterator InputIterator>
    [[nodiscard]] inline bool tryPushRange(const InputIterator from, const InputIterator to) noexcept
        { return pushRangeImpl<false>(from, to); }

    /** @brief Push up to 'count' elements into the queue
     *  @return The number of extracted elements */
    template<std::input_iterator InputIterator>
    [[nodiscard]] inline std::size_t pushRange(const InputIterator from, const InputIterator to) noexcept
        { return pushRangeImpl<true>(from, to); }


    /** @brief Pop exactly 'count' elements from the queue
     *  @return Success on true */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] inline bool tryPopRange(const OutputIterator from, const OutputIterator to) noexcept
        { return popRangeImpl<false>(from, to); }

    /** @brief Pop up to 'count' elements from the queue
     *  @return The number of extracted elements */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] inline std::size_t popRange(const OutputIterator from, const OutputIterator to) noexcept
        { return popRangeImpl<true>(from, to); }


    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept;


    /** @brief Get the size of the queue */
    [[nodiscard]] std::size_t size(void) const noexcept;


private:
    alignas_cacheline std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer
    alignas_cacheline Cache _tailCache {}; // Cache accessed by consumer thread

    alignas_cacheline std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumer
    alignas_cacheline Cache _headCache {}; // Cache accessed by producer thread


    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;


    /** @brief Implementation of push range */
    template<bool AllowLess, std::input_iterator InputIterator>
    [[nodiscard]] std::size_t pushRangeImpl(const InputIterator from, const InputIterator to) noexcept;

    /** @brief Implementation of pop range */
    template<bool AllowLess, typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] std::size_t popRangeImpl(const OutputIterator from, const OutputIterator to) noexcept;
};

#include "SPSCQueue.ipp"