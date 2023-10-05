/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VertexInputAttribute
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct VertexInputAttribute;
};

/** @brief Describe how to create a vertex input attribute */
struct kF::GPU::VertexInputAttribute : public VkVertexInputAttributeDescription
{
    /** @brief Destructor */
    ~VertexInputAttribute(void) noexcept = default;

    /** @brief Initialize constructor */
    VertexInputAttribute(
            const std::uint32_t binding_,
            const std::uint32_t location_,
            const Format format_,
            const std::uint32_t offset_) noexcept
        : VkVertexInputAttributeDescription {
            .location = location_,
            .binding = binding_,
            .format = static_cast<VkFormat>(format_),
            .offset = offset_,
        } {}

    /** @brief POD semantics */
    VertexInputAttribute(const VertexInputAttribute &other) noexcept = default;
    VertexInputAttribute(VertexInputAttribute &&other) noexcept = default;
    VertexInputAttribute &operator=(const VertexInputAttribute &other) noexcept = default;
    VertexInputAttribute &operator=(VertexInputAttribute &&other) noexcept = default;
};
