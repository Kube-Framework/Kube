/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SpecializationInfo
 */

#pragma once

#include "SpecializationMapEntry.hpp"

namespace kF::GPU
{
    struct SpecializationInfo;
};

/** @brief Describe a shader specialization */
struct kF::GPU::SpecializationInfo : public VkSpecializationInfo
{
    /** @brief Destructor */
    ~SpecializationInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    SpecializationInfo(
            const SpecializationMapEntry * const mapBegin, const SpecializationMapEntry * const mapEnd,
            const void * const dataBegin, const void * const dataEnd) noexcept
        : VkSpecializationInfo {
            .mapEntryCount = Core::Distance<std::uint32_t>(mapBegin, mapEnd),
            .pMapEntries = mapBegin,
            .dataSize = static_cast<std::uint32_t>(
                std::distance(reinterpret_cast<const std::uint8_t *>(dataBegin), reinterpret_cast<const std::uint8_t *>(dataEnd))
            ),
            .pData = dataBegin
        } {}


    /** @brief POD semantics */
    SpecializationInfo(const SpecializationInfo &other) noexcept = default;
    SpecializationInfo(SpecializationInfo &&other) noexcept = default;
    SpecializationInfo &operator=(const SpecializationInfo &other) noexcept = default;
    SpecializationInfo &operator=(SpecializationInfo &&other) noexcept = default;
};
