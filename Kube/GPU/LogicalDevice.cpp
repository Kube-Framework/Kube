/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: LogicalDevice
 */

#include <cstring>
#include <algorithm>

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "LogicalDevice.hpp"

#if KUBE_PLATFORM_APPLE
# include <vulkan/vulkan_beta.h>
#endif

using namespace kF;

GPU::LogicalDevice::~LogicalDevice(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyDevice(handle(), nullptr);
}

void GPU::LogicalDevice::waitIdle(void) noexcept
{
    ::vkDeviceWaitIdle(handle());
}

GPU::LogicalDevice::LogicalDevice(void) noexcept
{
    // Setup queues
    auto queueInfos = parent().queueManager().registerQueues();
    Core::SmallVector<float, 4> priorities;
    for (auto &info : queueInfos) {
        if (info.queueCount != priorities.size())
            priorities.resize(info.queueCount, 1);
    }
    for (auto &info : queueInfos) {
        info.pQueuePriorities = priorities.data();
    }
    // Device features
    const auto deviceFeatures = getDeviceFeatures();
    // Setup device
    const VkDeviceCreateInfo deviceInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &deviceFeatures->features,
        .flags = VkDeviceCreateFlags(),
        .queueCreateInfoCount = static_cast<std::uint32_t>(queueInfos.size()),
        .pQueueCreateInfos = queueInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<std::uint32_t>(deviceFeatures->extensions.size()),
        .ppEnabledExtensionNames = deviceFeatures->extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    // Create device
    if (const auto res = ::vkCreateDevice(parent().physicalDevice(), &deviceInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::LogicalDevice:: Couldn't create logical device '", ErrorMessage(res), '\'');
    // Update queue handles
    parent().queueManager().retreiveQueuesHandlers();
#if KUBE_DEBUG_BUILD
    kFInfoRaw("[GPU] Logical Extensions: ");
    bool first = true;
    for (const auto &extension : deviceFeatures->extensions) {
        if (!first)
            kFInfoRaw(", ");
        else
            first = false;
        kFInfoRaw(extension);
    }
    kFInfo();
#endif
}

GPU::LogicalDevice::DeviceFeaturesPtr GPU::LogicalDevice::getDeviceFeatures(void) const noexcept
{
    // Query features
    auto deviceFeatures = DeviceFeaturesPtr::Make(DeviceFeatures {
        .extensions = getExtensions(),
        .indexingFeatures = PhysicalDeviceDescriptorIndexingFeatures {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = nullptr
        },
        .features = PhysicalDeviceFeatures2 {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = nullptr,
        }
    });
    deviceFeatures->features.pNext = &deviceFeatures->indexingFeatures;
    ::vkGetPhysicalDeviceFeatures2(parent().physicalDevice(), &deviceFeatures->features);

    // Device features
    deviceFeatures->features.features = PhysicalDeviceFeatures {
    };

    // Indexing features
    kFEnsure(deviceFeatures->indexingFeatures.shaderSampledImageArrayNonUniformIndexing,
        "GPU::LogicalDevice: 'Shader sampled image array non uniform indexing' is not available for the selected device");
    kFEnsure(deviceFeatures->indexingFeatures.descriptorBindingSampledImageUpdateAfterBind,
        "GPU::LogicalDevice: 'Descriptor binding sampled image update after bind' is not available for the selected device");
    kFEnsure(deviceFeatures->indexingFeatures.descriptorBindingUpdateUnusedWhilePending,
        "GPU::LogicalDevice: 'Descriptor binding update unused while pending' is not available for the selected device");
    kFEnsure(deviceFeatures->indexingFeatures.descriptorBindingPartiallyBound,
        "GPU::LogicalDevice: 'Partial descriptor binding bound' is not available for the selected device");

    deviceFeatures->indexingFeatures = PhysicalDeviceDescriptorIndexingFeatures {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = nullptr,
        .shaderSampledImageArrayNonUniformIndexing = true,
        .descriptorBindingSampledImageUpdateAfterBind = true,
        .descriptorBindingUpdateUnusedWhilePending = true,
        .descriptorBindingPartiallyBound = true,
    };

    deviceFeatures->features.features = PhysicalDeviceFeatures {
    };

    return deviceFeatures;
}

GPU::LogicalDevice::Extensions GPU::LogicalDevice::getExtensions(void) const noexcept
{
    Extensions extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
#if KUBE_PLATFORM_APPLE
        , VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
#endif
    };
    Core::Vector<VkExtensionProperties> properties;

    if (const auto res = Internal::FillVkContainer(properties, &::vkEnumerateDeviceExtensionProperties, parent().physicalDevice(), nullptr); res != VK_SUCCESS)
        kFAbort("GPU::LogicalDevice::getExtensions: Couldn't enumerate logical device extensions '", ErrorMessage(res), '\'');
    for (const auto &extension : extensions) {
        auto found = false;
        for (const auto &property : properties) {
            if (found = !std::strcmp(property.extensionName, extension); found)
                break;
        }
        if (!found)
            kFAbort("GPU::Renderer::getLogicalExtensions: Extension '", extension, "' not found");
    }
    return extensions;
}