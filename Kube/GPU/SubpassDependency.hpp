/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SubpassDependency
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct SubpassDependency;
};

/** @brief Describe how to create a vertex input binding */
struct kF::GPU::SubpassDependency : public VkSubpassDependency
{
    /** @brief Destructor */
    ~SubpassDependency(void) noexcept = default;

    /** @brief Initialize constructor */
    SubpassDependency(
            const std::uint32_t srcSubpass_, const std::uint32_t dstSubpass_,
            const PipelineStageFlags srcStageMask_, const PipelineStageFlags dstStageMask_,
            const AccessFlags srcAccessMask_, const AccessFlags dstAccessMask_,
            const DependencyFlags dependencyFlags_) noexcept
        : VkSubpassDependency {
            .srcSubpass = srcSubpass_,
            .dstSubpass = dstSubpass_,
            .srcStageMask = static_cast<VkPipelineStageFlags>(srcStageMask_),
            .dstStageMask = static_cast<VkPipelineStageFlags>(dstStageMask_),
            .srcAccessMask = static_cast<VkAccessFlags>(srcAccessMask_),
            .dstAccessMask = static_cast<VkAccessFlags>(dstAccessMask_),
            .dependencyFlags = static_cast<VkDependencyFlags>(dependencyFlags_)
        } {}

    /** @brief POD semantics */
    SubpassDependency(const SubpassDependency &other) noexcept = default;
    SubpassDependency(SubpassDependency &&other) noexcept = default;
    SubpassDependency &operator=(const SubpassDependency &other) noexcept = default;
    SubpassDependency &operator=(SubpassDependency &&other) noexcept = default;
};
