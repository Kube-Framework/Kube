/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ShaderStageModel
 */

#pragma once

#include "SpecializationInfo.hpp"

namespace kF::GPU
{
    struct ShaderStageModel;
};

/** @brief Describe how to create a shader stage */
struct kF::GPU::ShaderStageModel : public VkPipelineShaderStageCreateInfo
{
    /** @brief Destructor */
    ~ShaderStageModel(void) noexcept = default;

    /** @brief Initialize constructor */
    ShaderStageModel(const ShaderStageFlags stage_, const ShaderModuleHandle module_,
            const SpecializationInfo * const pSpecializationInfo_ = nullptr, const char * const pName_ = "main") noexcept
        : VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(ShaderStageCreateFlags::None),
            .stage = static_cast<VkShaderStageFlagBits>(stage_),
            .module = module_,
            .pName = pName_,
            .pSpecializationInfo = pSpecializationInfo_
        } {}


    /** @brief POD semantics */
    ShaderStageModel(const ShaderStageModel &other) noexcept = default;
    ShaderStageModel(ShaderStageModel &&other) noexcept = default;
    ShaderStageModel &operator=(const ShaderStageModel &other) noexcept = default;
    ShaderStageModel &operator=(ShaderStageModel &&other) noexcept = default;
};
