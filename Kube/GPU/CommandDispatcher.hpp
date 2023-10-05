/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandDispatcher
 */

#pragma once

#include <optional>

#include "PerFrameCache.hpp"
#include "Semaphore.hpp"
#include "Fence.hpp"
#include "SubmitInfo.hpp"
#include "CommandPool.hpp"

namespace kF::GPU
{
    class CommandDispatcher;
}

/** @brief Command dispatcher collect commands and dispatch them later */
class alignas_cacheline kF::GPU::CommandDispatcher : public CachedGPUObject
{
public:
    /** @brief Cache of semaphore handle */
    using SemaphoreCache = Core::Vector<SemaphoreHandle, GPUAllocator>;

    /** @brief Cache of fence handle */
    using FenceCache = Core::Vector<FenceHandle, GPUAllocator>;

    /** @brief An array of command sorted by queue types */
    struct alignas_double_cacheline FrameCache
    {
        std::array<SemaphoreCache, QueueCount> perQueueSemaphoreCache {};
        std::array<FenceCache, QueueCount> perQueueFenceCache {};
        SemaphoreCache semaphoreCache {};
        FenceCache fenceCache {};
        std::optional<Semaphore> frameAvailable {};

        FrameCache(void) noexcept = default; // Default constructor not existing, optional bug ?
    };

    static_assert_alignof_double_cacheline(FrameCache);
    static_assert_sizeof(FrameCache, Core::CacheLineSize * 4);


    /** @brief Destruct the command dispatcher */
    ~CommandDispatcher(void) noexcept = default;

    /** @brief Construct the command dispatcher */
    CommandDispatcher(void) noexcept;

    /** @brief Move constructor */
    CommandDispatcher(CommandDispatcher &&other) noexcept = default;

    /** @brief Move assignment */
    CommandDispatcher &operator=(CommandDispatcher &&other) noexcept = default;


    /** @brief Get the frame available semaphore of the current frame */
    [[nodiscard]] inline SemaphoreHandle currentFrameAvailableSemaphore(void) const noexcept
        { return _cachedFrames.current().frameAvailable.value().handle(); }


    /** @brief Dispatch a single command of a given queue with fence synchronization
     *  @note Only thread safe if the underlying hardward queue is not already submitting */
    inline void dispatch(const QueueType queueType, const SubmitInfo &submit, const FenceHandle fence = NullHandle) noexcept
        { dispatch(queueType, &submit, &submit + 1, fence); }

    /** @brief Dispatch commands of a given queue with fence synchronization
     *  @note Only thread safe if the underlying hardward queue is not already submitting */
    void dispatch(
        const QueueType queueType,
        const SubmitInfo * const submitBegin,
        const SubmitInfo * const submitEnd,
        const FenceHandle fence = NullHandle
    ) noexcept;

    /** @brief Dispatch commands of a given queue with fence synchronization using initializer-list
     *  @note Only thread safe if the underlying hardward queue is not already submitting */
    inline void dispatch(const QueueType queueType, const std::initializer_list<SubmitInfo> &submits, const FenceHandle fence = NullHandle) noexcept
        { dispatch(queueType, submits.begin(), submits.end(), fence); }

    /** @brief Dispatch commands of a given queue with fence synchronization using a single inlined submit info as initializer-lists
     *  @note Only thread safe if the underlying hardward queue is not already submitting */
    inline void dispatch(
        const QueueType queueType,
        const std::initializer_list<CommandHandle> &commands,
        const std::initializer_list<SemaphoreHandle> &waits,
        const std::initializer_list<PipelineStageFlags> &waitStages,
        const std::initializer_list<SemaphoreHandle> &signals = {},
        const FenceHandle fence = NullHandle
    ) noexcept
        { dispatch(queueType, SubmitInfo(commands.begin(), commands.end(), waits.begin(), waits.end(), waitStages.begin(), waitStages.end(), signals.begin(), signals.end()), fence); }


    /** @brief Add single semaphore and fence dependencies to presentation of current frame
     *  @note Thread safe for different queue type calls */
    inline void addPresentDependencies(const QueueType queueType, const SemaphoreHandle semaphore, const FenceHandle fence) noexcept
        { addPresentDependencies(queueType, &semaphore, &semaphore + 1, &fence, &fence + 1); }

    /** @brief Add single semaphore to presentation of current frame
     *  @note Thread safe for different queue type calls */
    inline void addPresentDependencies(const QueueType queueType, const SemaphoreHandle semaphore) noexcept
        { _cachedFrames.current().perQueueSemaphoreCache[static_cast<std::size_t>(queueType)].push(semaphore); }

    /** @brief Add single fence to presentation of current frame
     *  @note Thread safe for different queue type calls */
    inline void addPresentDependencies(const QueueType queueType, const FenceHandle fence) noexcept
        { _cachedFrames.current().perQueueFenceCache[static_cast<std::size_t>(queueType)].push(fence); }

    /** @brief Add dependencies to presentation of current frame
     *  @note Thread safe for different queue type calls */
    void addPresentDependencies(
        const QueueType queueType,
        const SemaphoreHandle * const semaphoreBegin,
        const SemaphoreHandle *semaphoreEnd,
        const FenceHandle * const fenceBegin,
        const FenceHandle *fenceEnd
    ) noexcept;


    /** @brief Try to acquire the next frame */
    [[nodiscard]] bool tryAcquireNextFrame(void) noexcept;

    /** @brief Prepare current presentation */
    void presentFrame(void) noexcept;

private:
    Core::Vector<Semaphore, GPUAllocator> _availableSemaphores {};
    PerFrameCache<FrameCache, GPUAllocator> _cachedFrames;
};
static_assert_fit_cacheline(kF::GPU::CommandDispatcher);

#include "CommandDispatcher.ipp"