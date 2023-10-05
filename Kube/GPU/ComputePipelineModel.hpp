/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PipelineModel
 */

#pragma once

#include "ShaderStageModel.hpp"

namespace kF::GPU
{
    struct ComputePipelineModel;
};

/** @brief Describe how to create a compute pipeline */
struct kF::GPU::ComputePipelineModel : public VkComputePipelineCreateInfo
{
    /** @brief Destructor */
    ~ComputePipelineModel(void) noexcept = default;

    /** @brief Initialize constructor */
    ComputePipelineModel(const PipelineCreateFlags flags_,
            const ShaderStageModel &stage_,
            const PipelineLayoutHandle layout_,
            const PipelineHandle basePipelineHandle_ = NullHandle,
            const std::int32_t basePipelineIndex_ = 0u) noexcept
        : VkComputePipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .stage = stage_,
            .layout = layout_,
            .basePipelineHandle = basePipelineHandle_,
            .basePipelineIndex = basePipelineIndex_
        } {}

    /** @brief POD semantics */
    ComputePipelineModel(const ComputePipelineModel &other) noexcept = default;
    ComputePipelineModel(ComputePipelineModel &&other) noexcept = default;
    ComputePipelineModel &operator=(const ComputePipelineModel &other) noexcept = default;
    ComputePipelineModel &operator=(ComputePipelineModel &&other) noexcept = default;
};
