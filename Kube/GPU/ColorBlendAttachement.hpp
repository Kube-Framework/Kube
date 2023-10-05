/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ColorBlendAttachment
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ColorBlendAttachment;
};

/** @brief Describe how to create color blend attachment */
struct kF::GPU::ColorBlendAttachment : public VkPipelineColorBlendAttachmentState
{
    /** @brief Destructor */
    ~ColorBlendAttachment(void) noexcept = default;

    /** @brief Initialize constructor */
    ColorBlendAttachment(
            const bool blendEnable_ = false,
            const BlendFactor srcColorBlendFactor_ = BlendFactor::One,
            const BlendFactor dstColorBlendFactor_ = BlendFactor::Zero,
            const BlendOp colorBlendOp_ = BlendOp::Add,
            const BlendFactor srcAlphaBlendFactor_ = BlendFactor::One,
            const BlendFactor dstAlphaBlendFactor_ = BlendFactor::Zero,
            const BlendOp alphaBlendOp_ = BlendOp::Add,
            const ColorComponentFlags colorWriteMask_ = ColorComponentFlags::RGBA) noexcept
        : VkPipelineColorBlendAttachmentState {
            .blendEnable = blendEnable_,
            .srcColorBlendFactor = static_cast<VkBlendFactor>(srcColorBlendFactor_),
            .dstColorBlendFactor = static_cast<VkBlendFactor>(dstColorBlendFactor_),
            .colorBlendOp = static_cast<VkBlendOp>(colorBlendOp_),
            .srcAlphaBlendFactor = static_cast<VkBlendFactor>(srcAlphaBlendFactor_),
            .dstAlphaBlendFactor = static_cast<VkBlendFactor>(dstAlphaBlendFactor_),
            .alphaBlendOp = static_cast<VkBlendOp>(alphaBlendOp_),
            .colorWriteMask = ToFlags(colorWriteMask_),
        } {}


    /** @brief POD semantics */
    ColorBlendAttachment(const ColorBlendAttachment &other) noexcept = default;
    ColorBlendAttachment(ColorBlendAttachment &&other) noexcept = default;
    ColorBlendAttachment &operator=(const ColorBlendAttachment &other) noexcept = default;
    ColorBlendAttachment &operator=(ColorBlendAttachment &&other) noexcept = default;
};
