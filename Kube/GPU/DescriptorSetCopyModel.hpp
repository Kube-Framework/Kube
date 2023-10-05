/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetCopy
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DescriptorSetCopyModel;
};

/** @brief Describe how to create descriptor update copy command */
struct kF::GPU::DescriptorSetCopyModel : public VkCopyDescriptorSet
{
    /** @brief Destructor */
    ~DescriptorSetCopyModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorSetCopyModel(
            const DescriptorSetHandle srcSet_, const std::uint32_t srcBinding_, const std::uint32_t srcArrayElement_,
            const DescriptorSetHandle dstSet_, const std::uint32_t dstBinding_, const std::uint32_t dstArrayElement_,
            const std::uint32_t descriptorCount_) noexcept
        : VkCopyDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
            .pNext = nullptr,
            .srcSet = srcSet_,
            .srcBinding = srcBinding_,
            .srcArrayElement = srcArrayElement_,
            .dstSet = dstSet_,
            .dstBinding = dstBinding_,
            .dstArrayElement = dstArrayElement_,
            .descriptorCount = descriptorCount_
        } {}

    /** @brief POD semantics */
    DescriptorSetCopyModel(const DescriptorSetCopyModel &other) noexcept = default;
    DescriptorSetCopyModel(DescriptorSetCopyModel &&other) noexcept = default;
    DescriptorSetCopyModel &operator=(const DescriptorSetCopyModel &other) noexcept = default;
    DescriptorSetCopyModel &operator=(DescriptorSetCopyModel &&other) noexcept = default;

};
