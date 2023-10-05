/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: TessellationModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct TessellationModel;
};

/** @brief Describe how to create tesselation */
struct kF::GPU::TessellationModel : public VkPipelineTessellationStateCreateInfo
{
    /** @brief Destructor */
    ~TessellationModel(void) noexcept = default;

    /** @brief Initialize constructor */
    TessellationModel(const std::uint32_t patchControlPoints_ = 0u) noexcept
        : VkPipelineTessellationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(TessellationCreateFlags::None),
            .patchControlPoints = patchControlPoints_
        } {}

    /** @brief POD semantics */
    TessellationModel(const TessellationModel &other) noexcept = default;
    TessellationModel(TessellationModel &&other) noexcept = default;
    TessellationModel &operator=(const TessellationModel &other) noexcept = default;
    TessellationModel &operator=(TessellationModel &&other) noexcept = default;
};
