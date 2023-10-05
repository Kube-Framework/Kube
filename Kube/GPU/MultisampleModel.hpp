/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MultisampleModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct MultisampleModel;
};

/** @brief Describe how to create multisample */
struct kF::GPU::MultisampleModel : public VkPipelineMultisampleStateCreateInfo
{
    /** @brief Destructor */
    ~MultisampleModel(void) noexcept = default;

    /** @brief Initialize constructor */
    MultisampleModel(const SampleCountFlags rasterizationSamples_ = SampleCountFlags::X1, const bool sampleShadingEnable_ = false,
            const float minSampleShading_ = 1.0f, const SampleMask * const pSampleMask_ = nullptr,
            const bool alphaToCoverageEnable_ = false, const bool alphaToOneEnable_ = false) noexcept
        : VkPipelineMultisampleStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(MultisampleCreateFlags::None),
            .rasterizationSamples = static_cast<VkSampleCountFlagBits>(rasterizationSamples_),
            .sampleShadingEnable = sampleShadingEnable_,
            .minSampleShading = minSampleShading_,
            .pSampleMask = pSampleMask_,
            .alphaToCoverageEnable = alphaToCoverageEnable_,
            .alphaToOneEnable = alphaToOneEnable_
        } {}


    /** @brief POD semantics */
    MultisampleModel(const MultisampleModel &other) noexcept = default;
    MultisampleModel(MultisampleModel &&other) noexcept = default;
    MultisampleModel &operator=(const MultisampleModel &other) noexcept = default;
    MultisampleModel &operator=(MultisampleModel &&other) noexcept = default;
};
