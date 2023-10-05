/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageSubresourceRange
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ImageSubresourceRange;
};

/** @brief Component mapping structure */
struct kF::GPU::ImageSubresourceRange : public VkImageSubresourceRange
{
    /** @brief Destructor */
    ~ImageSubresourceRange(void) noexcept = default;

    /** @brief Initialize constructor */
    ImageSubresourceRange(
        const ImageAspectFlags aspectMask_,
        const std::uint32_t baseMipLevel_ = 0u,
        const std::uint32_t levelCount_ = 1u,
        const std::uint32_t baseArrayLayer_ = 0u,
        const std::uint32_t layerCount_ = 1u) noexcept
        : VkImageSubresourceRange {
            .aspectMask = ToFlags(aspectMask_),
            .baseMipLevel = baseMipLevel_,
            .levelCount = levelCount_,
            .baseArrayLayer = baseArrayLayer_,
            .layerCount = layerCount_,
        } {}


    /** @brief POD semantics */
    ImageSubresourceRange(const ImageSubresourceRange &other) noexcept = default;
    ImageSubresourceRange(ImageSubresourceRange &&other) noexcept = default;
    ImageSubresourceRange &operator=(const ImageSubresourceRange &other) noexcept = default;
    ImageSubresourceRange &operator=(ImageSubresourceRange &&other) noexcept = default;
};
