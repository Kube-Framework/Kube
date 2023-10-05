/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RenderPassModel
 */

#pragma once

#include "AttachmentDescription.hpp"
#include "SubpassDescription.hpp"
#include "SubpassDependency.hpp"

namespace kF::GPU
{
    struct RenderPassModel;
};

/** @brief Describe how to create a RenderPass */
struct kF::GPU::RenderPassModel : public VkRenderPassCreateInfo
{
    /** @brief Destructor */
    ~RenderPassModel(void) noexcept = default;

    /** @brief Initialize constructor */
    RenderPassModel(const AttachmentDescription * const attachmentBegin, const AttachmentDescription * const attachmentEnd,
            const SubpassDescription * const subpassBegin, const SubpassDescription * const subpassEnd,
            const SubpassDependency * const dependencyBegin, const SubpassDependency * const dependencyEnd) noexcept
        : VkRenderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = VkRenderPassCreateFlags(),
            .attachmentCount = Core::Distance<std::uint32_t>(attachmentBegin, attachmentEnd),
            .pAttachments = attachmentBegin,
            .subpassCount = Core::Distance<std::uint32_t>(subpassBegin, subpassEnd),
            .pSubpasses = subpassBegin,
            .dependencyCount = Core::Distance<std::uint32_t>(dependencyBegin, dependencyEnd),
            .pDependencies = dependencyBegin
        } {}


    /** @brief POD semantics */
    RenderPassModel(const RenderPassModel &other) noexcept = default;
    RenderPassModel(RenderPassModel &&other) noexcept = default;
    RenderPassModel &operator=(const RenderPassModel &other) noexcept = default;
    RenderPassModel &operator=(RenderPassModel &&other) noexcept = default;
};
