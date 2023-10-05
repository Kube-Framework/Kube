/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ViewportModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ViewportModel;
};

/** @brief Describe how to create a viewport */
struct kF::GPU::ViewportModel : public VkPipelineViewportStateCreateInfo
{
    /** @brief Destructor */
    ~ViewportModel(void) noexcept = default;

    /** @brief Initialize constructor */
    ViewportModel(const Viewport * const viewportBegin, const Viewport * const viewportEnd,
            const Rect2D * const scissorBegin, const Rect2D * const scissorEnd) noexcept
        : VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(ViewportCreateFlags::None),
            .viewportCount = Core::Distance<std::uint32_t>(viewportBegin, viewportEnd),
            .pViewports = viewportBegin,
            .scissorCount = Core::Distance<std::uint32_t>(scissorBegin, scissorEnd),
            .pScissors = scissorBegin
        } {}


    /** @brief POD semantics */
    ViewportModel(const ViewportModel &other) noexcept = default;
    ViewportModel(ViewportModel &&other) noexcept = default;
    ViewportModel &operator=(const ViewportModel &other) noexcept = default;
    ViewportModel &operator=(ViewportModel &&other) noexcept = default;
};
