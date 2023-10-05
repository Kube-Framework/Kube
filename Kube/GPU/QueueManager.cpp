/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Queue Handler
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "QueueManager.hpp"

using namespace kF;

GPU::QueueManager::QueueManager(void) noexcept
{
    retreiveFamilyQueueIndexes();
}

void GPU::QueueManager::waitQueueIdle(const QueueType queueType) noexcept
{
    ::vkQueueWaitIdle(queue(queueType));
}

GPU::QueueManager::QueueCreateInfos GPU::QueueManager::registerQueues(void) noexcept
{
    QueueCreateInfos queues;

    for (std::size_t type = 0u; type != static_cast<std::size_t>(QueueType::Count); ++type) {
        QueueDescriptor descriptor;
        bool queueFound = true;
        if (_candidatesMap[type].empty()) [[unlikely]]
            kFAbort("GPU::QueueManager::registerQueues: Couldn't register unsupported queue type '", QueueTypeName(static_cast<QueueType>(type)));
        for (const auto &candidate : _candidatesMap[type]) {
            queueFound = true;
            descriptor.queueFamilyIndex = candidate.queueFamilyIndex;
            descriptor.queueIndex = 0u;
            // For each assigned queue
            for (std::size_t past = 0u; past != type; ++past) {
                auto &pastQueue = _array[past];
                // Ensure the queue family is different than candidate
                if (pastQueue.queueFamilyIndex != candidate.queueFamilyIndex) [[likely]]
                    continue;
                // If the family has enough queues, assign its index
                else if (pastQueue.queueIndex + 1 != candidate.queueCount)
                    descriptor.queueIndex = pastQueue.queueIndex + 1;
                // The family is fully occupied
                else [[unlikely]] {
                    descriptor.queueFamilyIndex = candidate.queueFamilyIndex;
                    descriptor.queueIndex = pastQueue.queueIndex;
                    queueFound = false;
                    break;
                }
            }
            if (queueFound)
                break;
        }
        // Ensure we found a unique candidate
        if (!queueFound) [[unlikely]]
            kFInfo("[GPU] GPU::QueueManager::registerQueues: Queue conflict '",
                    QueueTypeName(static_cast<QueueType>(type)), "', with family ",
                    descriptor.queueFamilyIndex, " and index ", descriptor.queueIndex);
        // Ensure the queue family is not in create info list
        bool isInserted = false;
        for (auto &queue : queues) {
            if (queue.queueFamilyIndex == descriptor.queueFamilyIndex) {
                if (queue.queueCount <= descriptor.queueIndex)
                    queue.queueCount = descriptor.queueIndex + 1;
                isInserted = true;
                break;
            }
        }
        // If the queue is not in create info list, insert it
        if (!isInserted) {
            queues.push(QueueCreateInfo {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = VkDeviceQueueCreateFlags(),
                .queueFamilyIndex = descriptor.queueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = nullptr
            });
        }
        _array[type] = descriptor;
    }
    return queues;
}

void GPU::QueueManager::retreiveQueuesHandlers(void) noexcept
{
    Core::Vector<QueueDescriptor *> past;

    for (auto &desc : _array) {
        auto it = std::find_if(past.begin(), past.end(), [&desc](const auto *p) {
            return desc.queueFamilyIndex == p->queueFamilyIndex && desc.queueIndex == p->queueIndex;
        });
        if (it != past.end()) {
            desc.queueHandle = (*it)->queueHandle;
            continue;
        }
        ::vkGetDeviceQueue(parent().logicalDevice(), desc.queueFamilyIndex, desc.queueIndex, &desc.queueHandle);
        past.push(&desc);
    }

#if KUBE_DEBUG_BUILD
    kFInfo("[GPU] Queues:");
    for (std::size_t type = 0ul; type < static_cast<std::size_t>(QueueType::Count); ++type) {
        kFInfo('\t', QueueTypeName(static_cast<QueueType>(type)));
        if (!_candidatesMap[type].empty()) {
            for (const auto &candidate : _candidatesMap[type]) {
                if (candidate.queueFamilyIndex == _array[type].queueFamilyIndex) {
                    kFInfo("\t\t'", candidate.queueFamilyIndex, "' (", candidate.queueCount, " available, index ", _array[type].queueIndex, " used)");
                } else
                    kFInfo("\t\t ", candidate.queueFamilyIndex, "  (", candidate.queueCount, " available)");
            }
        } else
            kFInfo(" (Unsupported)");
    }
#endif
}

void GPU::QueueManager::retreiveFamilyQueueIndexes(void) noexcept
{
    Core::Vector<VkQueueFamilyProperties> properties;
    std::uint32_t queueFamilyIndex = 0;
    VkBool32 isPresent = false;

    Internal::FillVkContainer(properties, &::vkGetPhysicalDeviceQueueFamilyProperties, parent().physicalDevice());
    for (auto &property : properties) {
        if (const auto res = ::vkGetPhysicalDeviceSurfaceSupportKHR(parent().physicalDevice(), queueFamilyIndex, parent().surface(), &isPresent); res != VK_SUCCESS)
            kFAbort("GPU::QueueManager::registerQueues: Couldn't get physical device surface support '", ErrorMessage(res), '\'');
        const QueueCandidate candidate {
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = property.queueCount
        };
        if (isPresent)
            _candidatesMap[Core::ToUnderlying(QueueType::Present)].push(candidate);
        if (property.queueFlags & ToFlags(QueueFlags::Graphics))
            _candidatesMap[Core::ToUnderlying(QueueType::Graphics)].push(candidate);
        if (property.queueFlags & ToFlags(QueueFlags::Compute))
            _candidatesMap[Core::ToUnderlying(QueueType::Compute)].push(candidate);
        if (property.queueFlags & ToFlags(QueueFlags::Transfer))
            _candidatesMap[Core::ToUnderlying(QueueType::Transfer)].push(candidate);
        ++queueFamilyIndex;
    }
}