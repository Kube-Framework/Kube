/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SubpassDescription
 */

#pragma once

#include "AttachmentReference.hpp"

namespace kF::GPU
{
    struct SubpassDescription;
};

/** @brief Describe how to create a vertex input binding */
struct kF::GPU::SubpassDescription : public VkSubpassDescription
{
    /** @brief Destructor */
    ~SubpassDescription(void) noexcept = default;

    /** @brief Initialize constructor */
    SubpassDescription(
            const PipelineBindPoint pipelineBindPoint_,
            const AttachmentReference * const colorAttachmentBegin, const AttachmentReference * const colorAttachmentEnd,
            const AttachmentReference * const inputAttachmentBegin = nullptr, const AttachmentReference * const inputAttachmentEnd = nullptr,
            const AttachmentReference * const depthStencilAttachment = nullptr,
            const std::uint32_t * const preserveAttachmentBegin = nullptr, const std::uint32_t * const preserveAttachmentEnd = nullptr) noexcept
        : VkSubpassDescription {
            .flags = VkSubpassDescriptionFlags(),
            .pipelineBindPoint = static_cast<VkPipelineBindPoint>(pipelineBindPoint_),
            .inputAttachmentCount = Core::Distance<std::uint32_t>(inputAttachmentBegin, inputAttachmentEnd),
            .pInputAttachments = inputAttachmentBegin,
            .colorAttachmentCount = Core::Distance<std::uint32_t>(colorAttachmentBegin, colorAttachmentEnd),
            .pColorAttachments = colorAttachmentBegin,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = depthStencilAttachment,
            .preserveAttachmentCount = Core::Distance<std::uint32_t>(preserveAttachmentBegin, preserveAttachmentEnd),
            .pPreserveAttachments = preserveAttachmentBegin
        } {}

    /** @brief POD semantics */
    SubpassDescription(const SubpassDescription &other) noexcept = default;
    SubpassDescription(SubpassDescription &&other) noexcept = default;
    SubpassDescription &operator=(const SubpassDescription &other) noexcept = default;
    SubpassDescription &operator=(SubpassDescription &&other) noexcept = default;
};
