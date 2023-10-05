/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SamplerModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct SamplerModel;
};

/** @brief Describe how to create a sampler */
struct kF::GPU::SamplerModel : public VkSamplerCreateInfo
{
    /** @brief Destructor */
    ~SamplerModel(void) noexcept = default;

    /** @brief Initialize constructor */
    SamplerModel(const SamplerCreateFlags flags_,
        const Filter magFilter_,
        const Filter minFilter_,
        const SamplerMipmapMode mipmapMode_,
        const SamplerAddressMode addressModeU_,
        const SamplerAddressMode addressModeV_,
        const SamplerAddressMode addressModeW_,
        const bool anisotropyEnable_,
        const float maxAnisotropy_,
        const bool compareEnable_,
        const CompareOp compareOp_,
        const float mipLodBias_,
        const float minLod_,
        const float maxLod_,
        const BorderColor borderColor_,
        const bool unnormalizedCoordinates_
    ) noexcept
        : VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .magFilter = static_cast<VkFilter>(magFilter_),
            .minFilter = static_cast<VkFilter>(minFilter_),
            .mipmapMode = static_cast<VkSamplerMipmapMode>(mipmapMode_),
            .addressModeU = static_cast<VkSamplerAddressMode>(addressModeU_),
            .addressModeV = static_cast<VkSamplerAddressMode>(addressModeV_),
            .addressModeW = static_cast<VkSamplerAddressMode>(addressModeW_),
            .mipLodBias = mipLodBias_,
            .anisotropyEnable = anisotropyEnable_,
            .maxAnisotropy = maxAnisotropy_,
            .compareEnable = compareEnable_,
            .compareOp = static_cast<VkCompareOp>(compareOp_),
            .minLod = minLod_,
            .maxLod = maxLod_,
            .borderColor = static_cast<VkBorderColor>(borderColor_),
            .unnormalizedCoordinates = unnormalizedCoordinates_
        } {}


    /** @brief POD semantics */
    SamplerModel(const SamplerModel &other) noexcept = default;
    SamplerModel(SamplerModel &&other) noexcept = default;
    SamplerModel &operator=(const SamplerModel &other) noexcept = default;
    SamplerModel &operator=(SamplerModel &&other) noexcept = default;
};
