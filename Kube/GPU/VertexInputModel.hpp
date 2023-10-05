/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VertexInputModel
 */

#pragma once

#include "VertexInputBinding.hpp"
#include "VertexInputAttribute.hpp"

namespace kF::GPU
{
    struct VertexInputModel;
};

/** @brief Describe how to create vertex input */
struct kF::GPU::VertexInputModel : public VkPipelineVertexInputStateCreateInfo
{
    /** @brief Destructor */
    ~VertexInputModel(void) noexcept = default;

    /** @brief Initialize constructor */
    VertexInputModel(
            const VertexInputBinding * const bindingBegin = nullptr, const VertexInputBinding * const bindingEnd = nullptr,
            const VertexInputAttribute * const attributeBegin = nullptr, const VertexInputAttribute * const attributeEnd = nullptr) noexcept
        : VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(VertexInputCreateFlags::None),
            .vertexBindingDescriptionCount = Core::Distance<std::uint32_t>(bindingBegin, bindingEnd),
            .pVertexBindingDescriptions = bindingBegin,
            .vertexAttributeDescriptionCount = Core::Distance<std::uint32_t>(attributeBegin, attributeEnd),
            .pVertexAttributeDescriptions = attributeBegin
        } {}


    /** @brief POD semantics */
    VertexInputModel(const VertexInputModel &other) noexcept = default;
    VertexInputModel(VertexInputModel &&other) noexcept = default;
    VertexInputModel &operator=(const VertexInputModel &other) noexcept = default;
    VertexInputModel &operator=(VertexInputModel &&other) noexcept = default;
};
