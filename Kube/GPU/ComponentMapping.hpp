/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ComponentMapping
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ComponentMapping;
};

/** @brief Component mapping structure */
struct kF::GPU::ComponentMapping : public VkComponentMapping
{
    /** @brief Destructor */
    ~ComponentMapping(void) noexcept = default;

    /** @brief Initialize constructor */
    ComponentMapping(
        const ComponentSwizzle r_ = ComponentSwizzle::Identity,
        const ComponentSwizzle g_ = ComponentSwizzle::Identity,
        const ComponentSwizzle b_ = ComponentSwizzle::Identity,
        const ComponentSwizzle a_ = ComponentSwizzle::Identity) noexcept
        : VkComponentMapping {
            .r = static_cast<VkComponentSwizzle>(r_),
            .g = static_cast<VkComponentSwizzle>(g_),
            .b = static_cast<VkComponentSwizzle>(b_),
            .a = static_cast<VkComponentSwizzle>(a_)
        } {}


    /** @brief POD semantics */
    ComponentMapping(const ComponentMapping &other) noexcept = default;
    ComponentMapping(ComponentMapping &&other) noexcept = default;
    ComponentMapping &operator=(const ComponentMapping &other) noexcept = default;
    ComponentMapping &operator=(ComponentMapping &&other) noexcept = default;
};
