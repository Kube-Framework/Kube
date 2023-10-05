/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorBufferInfo
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DescriptorBufferInfo;
};

/** @brief Describe how to create a vertex input binding */
struct kF::GPU::DescriptorBufferInfo : public VkDescriptorBufferInfo
{
    /** @brief Destructor */
    ~DescriptorBufferInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorBufferInfo(const BufferHandle buffer_ = NullHandle, const BufferSize offset_ = 0u, const BufferSize range_ = VK_WHOLE_SIZE) noexcept
        : VkDescriptorBufferInfo {
            .buffer = buffer_,
            .offset = offset_,
            .range = range_
        } {}

    /** @brief POD semantics */
    DescriptorBufferInfo(const DescriptorBufferInfo &other) noexcept = default;
    DescriptorBufferInfo(DescriptorBufferInfo &&other) noexcept = default;
    DescriptorBufferInfo &operator=(const DescriptorBufferInfo &other) noexcept = default;
    DescriptorBufferInfo &operator=(DescriptorBufferInfo &&other) noexcept = default;
};
