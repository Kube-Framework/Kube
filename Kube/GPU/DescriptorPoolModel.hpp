/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorPoolModel
 */

#pragma once

#include "Base.hpp"
#include "DescriptorPoolSize.hpp"

namespace kF::GPU
{
    struct DescriptorPoolModel;
};

/** @brief Describe how to create an image */
struct kF::GPU::DescriptorPoolModel : public VkDescriptorPoolCreateInfo
{
    /** @brief Destructor */
    ~DescriptorPoolModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorPoolModel(
        const DescriptorPoolCreateFlags flags_,
        const std::uint32_t maxSets_,
        const DescriptorPoolSize * const poolSizeBegin,
        const DescriptorPoolSize * const poolSizeEnd
    ) noexcept
        : VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .maxSets = maxSets_,
            .poolSizeCount = Core::Distance<std::uint32_t>(poolSizeBegin, poolSizeEnd),
            .pPoolSizes = poolSizeBegin,
        } {}


    /** @brief POD semantics */
    DescriptorPoolModel(const DescriptorPoolModel &other) noexcept = default;
    DescriptorPoolModel(DescriptorPoolModel &&other) noexcept = default;
    DescriptorPoolModel &operator=(const DescriptorPoolModel &other) noexcept = default;
    DescriptorPoolModel &operator=(DescriptorPoolModel &&other) noexcept = default;
};
