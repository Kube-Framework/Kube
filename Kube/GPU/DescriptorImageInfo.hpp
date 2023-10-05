/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorImageInfo
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DescriptorImageInfo;
};

/** @brief Describe how to create a vertex input binding */
struct kF::GPU::DescriptorImageInfo : public VkDescriptorImageInfo
{
    /** @brief Destructor */
    ~DescriptorImageInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorImageInfo(
            const SamplerHandle sampler_ = NullHandle,
            const ImageViewHandle imageView_ = NullHandle,
            const ImageLayout imageLayout_ = ImageLayout::Undefined) noexcept
        : VkDescriptorImageInfo {
            .sampler = sampler_,
            .imageView = imageView_,
            .imageLayout = static_cast<VkImageLayout>(imageLayout_)
        } {}

    /** @brief POD semantics */
    DescriptorImageInfo(const DescriptorImageInfo &other) noexcept = default;
    DescriptorImageInfo(DescriptorImageInfo &&other) noexcept = default;
    DescriptorImageInfo &operator=(const DescriptorImageInfo &other) noexcept = default;
    DescriptorImageInfo &operator=(DescriptorImageInfo &&other) noexcept = default;
};
