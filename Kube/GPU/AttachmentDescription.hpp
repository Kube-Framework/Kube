/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AttachmentDescription
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct AttachmentDescription;
};

/** @brief Describe how to create a vertex input binding */
struct kF::GPU::AttachmentDescription : public VkAttachmentDescription
{
    /** @brief Destructor */
    ~AttachmentDescription(void) noexcept = default;

    /** @brief Initialize constructor */
    AttachmentDescription(
            const AttachmentDescriptionFlags flags_,
            const Format format_,
            const SampleCountFlags samples_,
            const AttachmentLoadOp loadOp_, const AttachmentStoreOp storeOp_,
            const AttachmentLoadOp stencilLoadOp_, const AttachmentStoreOp stencilStoreOp_,
            const ImageLayout initialLayout_, const ImageLayout finalLayout_) noexcept
        : VkAttachmentDescription {
            .flags = static_cast<VkAttachmentDescriptionFlags>(flags_),
            .format = static_cast<VkFormat>(format_),
            .samples = static_cast<VkSampleCountFlagBits>(samples_),
            .loadOp = static_cast<VkAttachmentLoadOp>(loadOp_),
            .storeOp = static_cast<VkAttachmentStoreOp>(storeOp_),
            .stencilLoadOp = static_cast<VkAttachmentLoadOp>(stencilLoadOp_),
            .stencilStoreOp = static_cast<VkAttachmentStoreOp>(stencilStoreOp_),
            .initialLayout = static_cast<VkImageLayout>(initialLayout_),
            .finalLayout = static_cast<VkImageLayout>(finalLayout_)
        } {}

    /** @brief POD semantics */
    AttachmentDescription(const AttachmentDescription &other) noexcept = default;
    AttachmentDescription(AttachmentDescription &&other) noexcept = default;
    AttachmentDescription &operator=(const AttachmentDescription &other) noexcept = default;
    AttachmentDescription &operator=(AttachmentDescription &&other) noexcept = default;
};
