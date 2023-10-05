/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetLayoutBindingFlagsModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DescriptorSetLayoutBindingFlagsModel;
};

/** @brief Describe how to create an image */
struct kF::GPU::DescriptorSetLayoutBindingFlagsModel : public VkDescriptorSetLayoutBindingFlagsCreateInfo
{
    /** @brief Destructor */
    ~DescriptorSetLayoutBindingFlagsModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorSetLayoutBindingFlagsModel(const DescriptorBindingFlags * const flagsFrom, const DescriptorBindingFlags * const flagsTo) noexcept
        : VkDescriptorSetLayoutBindingFlagsCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = Core::Distance<std::uint32_t>(flagsFrom, flagsTo),
            .pBindingFlags = reinterpret_cast<const VkDescriptorBindingFlags *>(flagsFrom)
        } {}


    /** @brief POD semantics */
    DescriptorSetLayoutBindingFlagsModel(const DescriptorSetLayoutBindingFlagsModel &other) noexcept = default;
    DescriptorSetLayoutBindingFlagsModel(DescriptorSetLayoutBindingFlagsModel &&other) noexcept = default;
    DescriptorSetLayoutBindingFlagsModel &operator=(const DescriptorSetLayoutBindingFlagsModel &other) noexcept = default;
    DescriptorSetLayoutBindingFlagsModel &operator=(DescriptorSetLayoutBindingFlagsModel &&other) noexcept = default;
};
