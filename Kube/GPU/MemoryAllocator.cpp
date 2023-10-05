/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocator
 */

#define VMA_IMPLEMENTATION
#include "VMA.hpp"

#include <Kube/Core/Abort.hpp>
#include <Kube/Core/Assert.hpp>

#include "GPU.hpp"
#include "MemoryAllocator.hpp"

using namespace kF;

GPU::MemoryAllocator::~MemoryAllocator(void) noexcept
{
    ::vmaDestroyAllocator(handle());
}

GPU::MemoryAllocator::MemoryAllocator(void) noexcept
{
    VmaAllocatorCreateInfo allocatorCreateInfo {
        .flags = VmaAllocatorCreateFlags(),
        .physicalDevice = parent().physicalDevice(),
        .device = parent().logicalDevice(),
        .preferredLargeHeapBlockSize = 0u, // 0 for default VkDeviceMemory block size = 256MB
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr, // NULL for no limit on maximum number of bytes that can be allocated on a particular Vulkan heap
        .pVulkanFunctions = nullptr,
        .instance = parent().instance(),
        .vulkanApiVersion = VK_API_VERSION_1_1,
    };

    if (const auto res = ::vmaCreateAllocator(&allocatorCreateInfo, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::MemoryAllocator: Couldn't create allocator '", ErrorMessage(res), '\'');
}

void GPU::MemoryAllocator::allocate(const MemoryAllocationModel * const modelFrom, const MemoryAllocationModel * const modelTo,
        MemoryAllocationHandle * const allocationFrom, [[maybe_unused]] MemoryAllocationHandle * const allocationTo) noexcept
{
    kFAssert(std::distance(modelFrom, modelTo) == std::distance(allocationFrom, allocationTo),
        "GPU::MemoryAllocator::allocate: Mismatching model count and allocation count");

    auto allocationIt = allocationFrom;
    for (auto modelIt = modelFrom; modelIt != modelTo; ++modelIt, ++allocationIt) {
        VmaAllocationCreateInfo allocationCreateInfo = {
            .flags = VmaAllocationCreateFlags(),
            .usage = static_cast<VmaMemoryUsage>(modelIt->memoryUsage()), // easy way to specify memory requirements
            .requiredFlags = 0u,
            .preferredFlags = 0u,
            .memoryTypeBits = 0u, // no restriction for memory types
            .pool = nullptr,
            .pUserData = nullptr
        };
        if (modelIt->bindType() == MemoryAllocationModel::BindType::Buffer) {
            auto res = ::vmaAllocateMemoryForBuffer(
                handle(),
                modelIt->buffer(),
                &allocationCreateInfo,
                allocationIt,
                nullptr
            );
            kFEnsure(res == VK_SUCCESS,
                "GPU::MemoryAllocator::allocate: Couldn't allocate memory for buffer '", ErrorMessage(res), '\'');
            res = ::vmaBindBufferMemory(handle(), *allocationIt, modelIt->buffer());
            kFEnsure(res == VK_SUCCESS,
                "GPU::MemoryAllocator::allocate: Couldn't bind memory to buffer '", ErrorMessage(res), '\'');
        } else {
            auto res = ::vmaAllocateMemoryForImage(
                handle(),
                modelIt->image(),
                &allocationCreateInfo,
                allocationIt,
                nullptr
            );
            kFEnsure(res == VK_SUCCESS,
                "GPU::MemoryAllocator::allocate: Couldn't allocate memory for image '", ErrorMessage(res), '\'');
            res = ::vmaBindImageMemory(handle(), *allocationIt, modelIt->image());
            kFEnsure(res == VK_SUCCESS,
                "GPU::MemoryAllocator::allocate: Couldn't bind memory to image '", ErrorMessage(res), '\'');
        }
    }
}

void GPU::MemoryAllocator::deallocate(
        const MemoryAllocationHandle * const allocationFrom, const MemoryAllocationHandle * const allocationTo) noexcept
{
    for (auto allocationIt = allocationFrom; allocationIt != allocationTo; ++allocationIt) {
        ::vmaFreeMemory(handle(), *allocationIt);
    }
}