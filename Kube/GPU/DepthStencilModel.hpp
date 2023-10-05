/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DepthStencilModel
 */

#pragma once

#include "StencilOpState.hpp"

namespace kF::GPU
{
    struct DepthStencilModel;
};

/** @brief Describe how to create a depth stencil */
struct kF::GPU::DepthStencilModel : public VkPipelineDepthStencilStateCreateInfo
{
    /** @brief Destructor */
    ~DepthStencilModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DepthStencilModel(const bool depthTestEnable_ = false, const bool depthWriteEnable_ = false,
            const CompareOp depthCompareOp_ = CompareOp::Never, const bool depthBoundsTestEnable_ = false,
            const float minDepthBounds_ = 0.0f, const float maxDepthBounds_ = 1.0f,
            const bool stencilTestEnable_ = false, const StencilOpState front_ = StencilOpState(),
            const StencilOpState back_ = StencilOpState()) noexcept
        : VkPipelineDepthStencilStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(DepthStencilCreateFlags::None),
            .depthTestEnable = depthTestEnable_,
            .depthWriteEnable = depthWriteEnable_,
            .depthCompareOp = static_cast<VkCompareOp>(depthCompareOp_),
            .depthBoundsTestEnable = depthBoundsTestEnable_,
            .stencilTestEnable = stencilTestEnable_,
            .front = front_,
            .back = back_,
            .minDepthBounds = minDepthBounds_,
            .maxDepthBounds = maxDepthBounds_
        } {}

    /** @brief POD semantics */
    DepthStencilModel(const DepthStencilModel &other) noexcept = default;
    DepthStencilModel(DepthStencilModel &&other) noexcept = default;
    DepthStencilModel &operator=(const DepthStencilModel &other) noexcept = default;
    DepthStencilModel &operator=(DepthStencilModel &&other) noexcept = default;
};
