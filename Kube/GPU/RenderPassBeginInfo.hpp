/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RenderPassBeginInfo
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct RenderPassBeginInfo;
};

/** @brief Describe how to create a renderpass begin command */
struct kF::GPU::RenderPassBeginInfo : public VkRenderPassBeginInfo
{
    /** @brief Destructor */
    ~RenderPassBeginInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    RenderPassBeginInfo(const RenderPassHandle renderPass_,
            const FramebufferHandle framebuffer_,
            const Rect2D renderArea_,
            const ClearValue * const clearBegin, const ClearValue * const clearEnd) noexcept
        : VkRenderPassBeginInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = renderPass_,
            .framebuffer = framebuffer_,
            .renderArea = renderArea_,
            .clearValueCount = Core::Distance<std::uint32_t>(clearBegin, clearEnd),
            .pClearValues = clearBegin
        } {}


    /** @brief POD semantics */
    RenderPassBeginInfo(const RenderPassBeginInfo &other) noexcept = default;
    RenderPassBeginInfo(RenderPassBeginInfo &&other) noexcept = default;
    RenderPassBeginInfo &operator=(const RenderPassBeginInfo &other) noexcept = default;
    RenderPassBeginInfo &operator=(RenderPassBeginInfo &&other) noexcept = default;
};
