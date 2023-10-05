/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Queue Handler
 */

#pragma once

#include <array>

#include <Kube/Core/Vector.hpp>
#include <Kube/Core/SmallVector.hpp>

#include "Base.hpp"
#include "GPUObject.hpp"

namespace kF::GPU
{
    class QueueManager;
}

/** @brief Handles a list of queues */
class alignas_double_cacheline kF::GPU::QueueManager : public CachedGPUObject
{
public:
    /** @brief Describes a queue family and handle */
    struct alignas_quarter_cacheline QueueDescriptor
    {
        std::uint32_t queueFamilyIndex {}; // Index of the queue family
        std::uint32_t queueIndex {}; // Index from specific queue family
        QueueHandle queueHandle {}; // Handle of the queue
    };
    static_assert_fit_quarter_cacheline(QueueDescriptor);

    /** @brief An array containing available queue descriptors */
    using QueuesArray = std::array<QueueDescriptor, QueueCount>;

    /** @brief Queue family candidate */
    struct QueueCandidate
    {
        std::uint32_t queueFamilyIndex {};
        std::uint32_t queueCount {};
    };

    /** @brief All candidates of a queue family */
    using QueueCandidates = Core::Vector<QueueCandidate, GPUAllocator>;

    /** @brief An array containing all available queue candidates */
    using QueueCandidateMap = std::array<QueueCandidates, QueueCount>;

    /** @brief Describe how to create a queue */
    using QueueCreateInfo = VkDeviceQueueCreateInfo;

    /** @brief Describe how to multiple queues */
    using QueueCreateInfos = Core::Vector<QueueCreateInfo, GPUAllocator>;


    /** @brief Destroy queue handler */
    ~QueueManager(void) noexcept = default;

    /** @brief Construct queue handler */
    QueueManager(void) noexcept;

    /** @brief Move constructor */
    QueueManager(QueueManager &&other) noexcept = default;

    /** @brief Move assignment */
    QueueManager &operator=(QueueManager &&other) noexcept = default;


    /** @brief Retreive a queue descriptor that match given type and index */
    [[nodiscard]] const QueueDescriptor &queueDescriptor(const QueueType type) const noexcept
        { return _array[static_cast<std::size_t>(type)]; }

    /** @brief Retreive a queue that match given type and index */
    [[nodiscard]] const QueueHandle &queue(const QueueType type) const noexcept
        { return _array[static_cast<std::size_t>(type)].queueHandle; }


    /** @brief Wait until a queue is IDLE */
    void waitQueueIdle(const QueueType queueType) noexcept;


    /**
     * @brief Get a create info list of queues
     *  This function tries to give as much different queues as the hardware can manage
     *  If there isn't enough different queues, it'll use the same queue to assume different queue command types
     */
    [[nodiscard]] QueueCreateInfos registerQueues(void) noexcept;

    /** @brief Fill internal queues handler using registered queues */
    void retreiveQueuesHandlers(void) noexcept;

private:
    QueuesArray _array {};
    QueueCandidateMap _candidatesMap {};

    /** @brief Retreive every family queue and indexes that the hardware is capable of */
    void retreiveFamilyQueueIndexes(void) noexcept;
};

static_assert_alignof_double_cacheline(kF::GPU::QueueManager);