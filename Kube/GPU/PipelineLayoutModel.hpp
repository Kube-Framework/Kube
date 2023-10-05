/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PipelineLayoutModel
 */

#pragma once

#include "PushConstantRange.hpp"

namespace kF::GPU
{
    struct PipelineLayoutModel;
};

/** @brief Describe how to create a pipeline layout */
struct kF::GPU::PipelineLayoutModel : public VkPipelineLayoutCreateInfo
{
    /** @brief Destructor */
    ~PipelineLayoutModel(void) noexcept = default;

    /** @brief Initialize constructor */
    PipelineLayoutModel(const DescriptorSetLayoutHandle * const setLayoutBegin, const DescriptorSetLayoutHandle * const setLayoutEnd,
            const PushConstantRange * const pushConstantBegin, const PushConstantRange * const pushConstantEnd) noexcept
        : VkPipelineLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(PipelineLayoutCreateFlags::None),
            .setLayoutCount = Core::Distance<std::uint32_t>(setLayoutBegin, setLayoutEnd),
            .pSetLayouts = setLayoutBegin,
            .pushConstantRangeCount = Core::Distance<std::uint32_t>(pushConstantBegin, pushConstantEnd),
            .pPushConstantRanges = pushConstantBegin
        } {}


    /** @brief POD semantics */
    PipelineLayoutModel(const PipelineLayoutModel &other) noexcept = default;
    PipelineLayoutModel(PipelineLayoutModel &&other) noexcept = default;
    PipelineLayoutModel &operator=(const PipelineLayoutModel &other) noexcept = default;
    PipelineLayoutModel &operator=(PipelineLayoutModel &&other) noexcept = default;
};