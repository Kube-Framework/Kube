/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RasterizationModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct RasterizationModel;
};

/** @brief Describe how to create rasterization */
struct kF::GPU::RasterizationModel : public VkPipelineRasterizationStateCreateInfo
{
    /** @brief Destructor */
    ~RasterizationModel(void) noexcept = default;

    /** @brief Initialize constructor */
    RasterizationModel(const PolygonMode polygonMode_,
            const CullModeFlags cullMode_ = CullModeFlags::None,
            const FrontFace frontFace_ = FrontFace::Clockwise,
            const bool depthBiasEnable_ = false, const float depthBiasConstantFactor_ = 0.0f,
            const float depthBiasClamp_ = 0.0f, const float depthBiasSlopeFactor_ = 0.0f,
            const bool depthClampEnable_ = false,
            const bool rasterizerDiscardEnable_ = false,
            const float lineWidth_ = 1.0f) noexcept
        : VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(RasterizationCreateFlags::None),
            .depthClampEnable = depthClampEnable_,
            .rasterizerDiscardEnable = rasterizerDiscardEnable_,
            .polygonMode = static_cast<VkPolygonMode>(polygonMode_),
            .cullMode = ToFlags(cullMode_),
            .frontFace = static_cast<VkFrontFace>(frontFace_),
            .depthBiasEnable = depthBiasEnable_,
            .depthBiasConstantFactor = depthBiasConstantFactor_,
            .depthBiasClamp = depthBiasClamp_,
            .depthBiasSlopeFactor = depthBiasSlopeFactor_,
            .lineWidth = lineWidth_
        } {}

    /** @brief POD semantics */
    RasterizationModel(const RasterizationModel &other) noexcept = default;
    RasterizationModel(RasterizationModel &&other) noexcept = default;
    RasterizationModel &operator=(const RasterizationModel &other) noexcept = default;
    RasterizationModel &operator=(RasterizationModel &&other) noexcept = default;
};
