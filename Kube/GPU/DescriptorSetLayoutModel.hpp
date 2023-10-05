/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetLayoutModel
 */

#pragma once

#include "Base.hpp"
#include "DescriptorSetLayoutBinding.hpp"

namespace kF::GPU
{
    struct DescriptorSetLayoutModel;
};

/** @brief Describe how to create an image */
struct kF::GPU::DescriptorSetLayoutModel : public VkDescriptorSetLayoutCreateInfo
{
    /** @brief Destructor */
    ~DescriptorSetLayoutModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorSetLayoutModel(
        const DescriptorSetLayoutCreateFlags flags_,
        const DescriptorSetLayoutBinding * const bindingBegin,
        const DescriptorSetLayoutBinding * const bindingEnd
    ) noexcept
        : VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .bindingCount = Core::Distance<std::uint32_t>(bindingBegin, bindingEnd),
            .pBindings = bindingBegin
        } {}


    /** @brief POD semantics */
    DescriptorSetLayoutModel(const DescriptorSetLayoutModel &other) noexcept = default;
    DescriptorSetLayoutModel(DescriptorSetLayoutModel &&other) noexcept = default;
    DescriptorSetLayoutModel &operator=(const DescriptorSetLayoutModel &other) noexcept = default;
    DescriptorSetLayoutModel &operator=(DescriptorSetLayoutModel &&other) noexcept = default;
};
