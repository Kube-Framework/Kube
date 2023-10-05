/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SpecializationMapEntry
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct SpecializationMapEntry;
};

/** @brief Describe a shader map entry */
struct kF::GPU::SpecializationMapEntry : public VkSpecializationMapEntry
{
    /** @brief Destructor */
    ~SpecializationMapEntry(void) noexcept = default;

    /** @brief Initialize constructor */
    SpecializationMapEntry(
            const std::uint32_t constantID_,
            const std::uint32_t offset_,
            const std::size_t size_) noexcept
        : VkSpecializationMapEntry {
            .constantID = constantID_,
            .offset = offset_,
            .size = size_
        } {}


    /** @brief POD semantics */
    SpecializationMapEntry(const SpecializationMapEntry &other) noexcept = default;
    SpecializationMapEntry(SpecializationMapEntry &&other) noexcept = default;
    SpecializationMapEntry &operator=(const SpecializationMapEntry &other) noexcept = default;
    SpecializationMapEntry &operator=(SpecializationMapEntry &&other) noexcept = default;
};
