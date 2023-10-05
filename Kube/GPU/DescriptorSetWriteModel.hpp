/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetWrite
 */

#pragma once

#include "Handle.hpp"
#include "DescriptorImageInfo.hpp"
#include "DescriptorBufferInfo.hpp"

namespace kF::GPU
{
    struct DescriptorSetWriteModel;
};


/** @brief Describe how to create descriptor update write command */
struct kF::GPU::DescriptorSetWriteModel : public VkWriteDescriptorSet
{
    /** @brief Destructor */
    ~DescriptorSetWriteModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DescriptorSetWriteModel(
        const DescriptorSetHandle dstSet_,
        const std::uint32_t dstBinding_,
        const std::uint32_t dstArrayElement_,
        const std::uint32_t descriptorCount_,
        const DescriptorType descriptorType_,
        const DescriptorImageInfo *pImageInfo_,
        const DescriptorBufferInfo *pBufferInfo_,
        const BufferViewHandle *pTexelBufferView_
    ) noexcept
        : VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = dstSet_,
            .dstBinding = dstBinding_,
            .dstArrayElement = dstArrayElement_,
            .descriptorCount = descriptorCount_,
            .descriptorType = static_cast<VkDescriptorType>(descriptorType_),
            .pImageInfo = pImageInfo_,
            .pBufferInfo = pBufferInfo_,
            .pTexelBufferView = pTexelBufferView_
        } {}

    /** @brief Initialize constructor (image only) */
    DescriptorSetWriteModel(
        const DescriptorSetHandle dstSet_,
        const std::uint32_t dstBinding_,
        const std::uint32_t dstArrayElement_,
        const DescriptorType descriptorType_,
        const DescriptorImageInfo *imageBegin,
        const DescriptorImageInfo *imageEnd
    ) noexcept
        : DescriptorSetWriteModel(
            dstSet_, dstBinding_, dstArrayElement_,
            Core::Distance<std::uint32_t>(imageBegin, imageEnd),
            descriptorType_,
            imageBegin,
            nullptr,
            nullptr
        ) {}

    /** @brief Initialize constructor (buffer only) */
    DescriptorSetWriteModel(
        const DescriptorSetHandle dstSet_,
        const std::uint32_t dstBinding_,
        const std::uint32_t dstArrayElement_,
        const DescriptorType descriptorType_,
        const DescriptorBufferInfo *bufferBegin,
        const DescriptorBufferInfo *bufferEnd
    ) noexcept
        : DescriptorSetWriteModel(
            dstSet_, dstBinding_, dstArrayElement_,
            Core::Distance<std::uint32_t>(bufferBegin, bufferEnd),
            descriptorType_,
            nullptr,
            bufferBegin,
            nullptr
        ) {}

    /** @brief Initialize constructor (texel only) */
    DescriptorSetWriteModel(
        const DescriptorSetHandle dstSet_,
        const std::uint32_t dstBinding_,
        const std::uint32_t dstArrayElement_,
        const DescriptorType descriptorType_,
        const BufferViewHandle *texelBufferBegin,
        const BufferViewHandle *texelBufferEnd
    ) noexcept
        : DescriptorSetWriteModel(
            dstSet_, dstBinding_, dstArrayElement_,
            Core::Distance<std::uint32_t>(texelBufferBegin, texelBufferEnd),
            descriptorType_,
            nullptr,
            nullptr,
            texelBufferBegin
        ) {}


    /** @brief POD semantics */
    DescriptorSetWriteModel(const DescriptorSetWriteModel &other) noexcept = default;
    DescriptorSetWriteModel(DescriptorSetWriteModel &&other) noexcept = default;
    DescriptorSetWriteModel &operator=(const DescriptorSetWriteModel &other) noexcept = default;
    DescriptorSetWriteModel &operator=(DescriptorSetWriteModel &&other) noexcept = default;
};
