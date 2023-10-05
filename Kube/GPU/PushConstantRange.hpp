/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PushConstantRange
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct PushConstantRange;
};

/** @brief Describe how to create an image */
struct kF::GPU::PushConstantRange : public VkPushConstantRange
{
    /** @brief Destructor */
    ~PushConstantRange(void) noexcept = default;

    /** @brief Initialize constructor */
    PushConstantRange(const ShaderStageFlags shaderStageFlags, const std::uint32_t offset, const std::uint32_t size) noexcept
        : VkPushConstantRange {
            .stageFlags = static_cast<VkShaderStageFlags>(shaderStageFlags),
            .offset = offset,
            .size = size,
        } {}


    /** @brief POD semantics */
    PushConstantRange(const PushConstantRange &other) noexcept = default;
    PushConstantRange(PushConstantRange &&other) noexcept = default;
    PushConstantRange &operator=(const PushConstantRange &other) noexcept = default;
    PushConstantRange &operator=(PushConstantRange &&other) noexcept = default;
};
