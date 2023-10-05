/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AttachmentReference
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct AttachmentReference;
};

/** @brief Describe how to create a buffer copy */
struct kF::GPU::AttachmentReference : public VkAttachmentReference
{
    /** @brief Destructor */
    ~AttachmentReference(void) noexcept = default;

    /** @brief Initialize constructor */
    AttachmentReference(const std::uint32_t attachment_, const ImageLayout layout_) noexcept
        : VkAttachmentReference {
            .attachment = attachment_,
            .layout = static_cast<VkImageLayout>(layout_)
        } {}

    /** @brief POD semantics */
    AttachmentReference(const AttachmentReference &other) noexcept = default;
    AttachmentReference(AttachmentReference &&other) noexcept = default;
    AttachmentReference &operator=(const AttachmentReference &other) noexcept = default;
    AttachmentReference &operator=(AttachmentReference &&other) noexcept = default;
};
