/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetLayoutBinding
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DescriptorSetLayoutBinding;
};

/** @brief Describe how to create an image */
struct kF::GPU::DescriptorSetLayoutBinding : public VkDescriptorSetLayoutBinding
{
    /** @brief Destructor */
    ~DescriptorSetLayoutBinding(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorSetLayoutBinding(const std::uint32_t binding_, const DescriptorType type_, const std::uint32_t descriptorCount_,
            const ShaderStageFlags shaderStageFlags_, const SamplerHandle * const samplers = nullptr) noexcept
        : VkDescriptorSetLayoutBinding {
            .binding = binding_,
            .descriptorType = static_cast<VkDescriptorType>(type_),
            .descriptorCount = descriptorCount_,
            .stageFlags = ToFlags(shaderStageFlags_),
            .pImmutableSamplers = samplers
        } {}


    /** @brief POD semantics */
    DescriptorSetLayoutBinding(const DescriptorSetLayoutBinding &other) noexcept = default;
    DescriptorSetLayoutBinding(DescriptorSetLayoutBinding &&other) noexcept = default;
    DescriptorSetLayoutBinding &operator=(const DescriptorSetLayoutBinding &other) noexcept = default;
    DescriptorSetLayoutBinding &operator=(DescriptorSetLayoutBinding &&other) noexcept = default;
};
