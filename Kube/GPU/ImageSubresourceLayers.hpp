/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageSubresourceLayers
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ImageSubresourceLayers;
};

/** @brief Describe how to create a buffer copy */
struct kF::GPU::ImageSubresourceLayers : public VkImageSubresourceLayers
{
    /** @brief Destructor */
    ~ImageSubresourceLayers(void) noexcept = default;

    /** @brief Initialize constructor */
    ImageSubresourceLayers(const ImageAspectFlags aspectMask_,
            const std::uint32_t mipLevel_ = 0u, const std::uint32_t baseArrayLayer_ = 0u, const std::uint32_t layerCount_ = 1u) noexcept
        : VkImageSubresourceLayers {
            .aspectMask = static_cast<VkImageAspectFlags>(aspectMask_),
            .mipLevel = mipLevel_,
            .baseArrayLayer = baseArrayLayer_,
            .layerCount = layerCount_
        } {}


    /** @brief POD semantics */
    ImageSubresourceLayers(const ImageSubresourceLayers &other) noexcept = default;
    ImageSubresourceLayers(ImageSubresourceLayers &&other) noexcept = default;
    ImageSubresourceLayers &operator=(const ImageSubresourceLayers &other) noexcept = default;
    ImageSubresourceLayers &operator=(ImageSubresourceLayers &&other) noexcept = default;
};
