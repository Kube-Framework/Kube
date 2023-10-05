/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Memory barrier
 */

#pragma once

#include "Base.hpp"
#include "ImageSubresourceRange.hpp"

namespace kF::GPU
{
    struct MemoryBarrier;
    struct BufferMemoryBarrier;
    struct ImageMemoryBarrier;
};

/** @brief Describe how to create a memory barrier */
struct kF::GPU::MemoryBarrier : public VkMemoryBarrier
{
    /** @brief Destructor */
    ~MemoryBarrier(void) noexcept = default;

    /** @brief Initialize constructor */
    MemoryBarrier(const AccessFlags source, const AccessFlags destination) noexcept
        : VkMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = static_cast<VkAccessFlags>(source),
            .dstAccessMask = static_cast<VkAccessFlags>(destination)
        } {}


    /** @brief POD semantics */
    MemoryBarrier(const MemoryBarrier &other) noexcept = default;
    MemoryBarrier(MemoryBarrier &&other) noexcept = default;
    MemoryBarrier &operator=(const MemoryBarrier &other) noexcept = default;
    MemoryBarrier &operator=(MemoryBarrier &&other) noexcept = default;
};

/** @brief Describe how to create a buffer memory barrier */
struct kF::GPU::BufferMemoryBarrier : public VkBufferMemoryBarrier
{
    /** @brief Destructor */
    ~BufferMemoryBarrier(void) noexcept = default;

    /** @brief Initialize constructor */
    BufferMemoryBarrier(
        const AccessFlags source,
        const AccessFlags destination,
        const std::uint32_t sourceQueueFamilyIndex,
        const std::uint32_t destinationQueueFamilyIndex,
        const BufferHandle buffer_,
        const BufferSize offset_,
        const BufferSize size_
    ) noexcept
        : VkBufferMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = static_cast<VkAccessFlags>(source),
            .dstAccessMask = static_cast<VkAccessFlags>(destination),
            .srcQueueFamilyIndex = sourceQueueFamilyIndex,
            .dstQueueFamilyIndex = destinationQueueFamilyIndex,
            .buffer = buffer_,
            .offset = offset_,
            .size = size_,
        } {}


    /** @brief POD semantics */
    BufferMemoryBarrier(const BufferMemoryBarrier &other) noexcept = default;
    BufferMemoryBarrier(BufferMemoryBarrier &&other) noexcept = default;
    BufferMemoryBarrier &operator=(const BufferMemoryBarrier &other) noexcept = default;
    BufferMemoryBarrier &operator=(BufferMemoryBarrier &&other) noexcept = default;
};


/** @brief Describe how to create a buffer memory barrier */
struct kF::GPU::ImageMemoryBarrier : public VkImageMemoryBarrier
{
    /** @brief Destructor */
    ~ImageMemoryBarrier(void) noexcept = default;

    /** @brief Initialize constructor */
    ImageMemoryBarrier(
        const AccessFlags source,
        const AccessFlags destination,
        const ImageLayout oldLayout_,
        const ImageLayout newLayout_,
        const std::uint32_t sourceQueueFamilyIndex,
        const std::uint32_t destinationQueueFamilyIndex,
        const ImageHandle image_,
        const ImageSubresourceRange &subresourceRange_
    ) noexcept
        : VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = static_cast<VkAccessFlags>(source),
            .dstAccessMask = static_cast<VkAccessFlags>(destination),
            .oldLayout = static_cast<VkImageLayout>(oldLayout_),
            .newLayout = static_cast<VkImageLayout>(newLayout_),
            .srcQueueFamilyIndex = sourceQueueFamilyIndex,
            .dstQueueFamilyIndex = destinationQueueFamilyIndex,
            .image = image_,
            .subresourceRange = subresourceRange_,
        } {}


    /** @brief POD semantics */
    ImageMemoryBarrier(const ImageMemoryBarrier &other) noexcept = default;
    ImageMemoryBarrier(ImageMemoryBarrier &&other) noexcept = default;
    ImageMemoryBarrier &operator=(const ImageMemoryBarrier &other) noexcept = default;
    ImageMemoryBarrier &operator=(ImageMemoryBarrier &&other) noexcept = default;
};
