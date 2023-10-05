/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: BufferImageCopy
 */

#pragma once

#include "ImageSubresourceLayers.hpp"

namespace kF::GPU
{
    struct BufferImageCopy;
};

/** @brief Describe how to create a buffer copy */
struct kF::GPU::BufferImageCopy : public VkBufferImageCopy
{
    /** @brief Destructor */
    ~BufferImageCopy(void) noexcept = default;

    /** @brief Initialize constructor */
    BufferImageCopy(const BufferSize bufferOffset_, const std::uint32_t bufferRowLength_, const std::uint32_t bufferImageHeight_,
            const ImageSubresourceLayers imageSubresource_, const Offset3D imageOffset_, const Extent3D imageExtent_) noexcept
        : VkBufferImageCopy {
            .bufferOffset = bufferOffset_,
            .bufferRowLength = bufferRowLength_,
            .bufferImageHeight = bufferImageHeight_,
            .imageSubresource = imageSubresource_,
            .imageOffset = imageOffset_,
            .imageExtent = imageExtent_
        } {}


    /** @brief POD semantics */
    BufferImageCopy(const BufferImageCopy &other) noexcept = default;
    BufferImageCopy(BufferImageCopy &&other) noexcept = default;
    BufferImageCopy &operator=(const BufferImageCopy &other) noexcept = default;
    BufferImageCopy &operator=(BufferImageCopy &&other) noexcept = default;
};
