/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DynamicStateModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct DynamicStateModel;
};

/** @brief Describe how to create a dynamic state */
struct kF::GPU::DynamicStateModel : public VkPipelineDynamicStateCreateInfo
{
    /** @brief Destructor */
    ~DynamicStateModel(void) noexcept = default;

    /** @brief Initialize constructor */
    DynamicStateModel(const DynamicState * const stateBegin = nullptr, const DynamicState * const stateEnd = nullptr)
        noexcept
        : VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(DynamicStateCreateFlags::None),
            .dynamicStateCount = Core::Distance<std::uint32_t>(stateBegin, stateEnd),
            .pDynamicStates = reinterpret_cast<const VkDynamicState *>(stateBegin)
        } {}

    /** @brief POD semantics */
    DynamicStateModel(const DynamicStateModel &other) noexcept = default;
    DynamicStateModel(DynamicStateModel &&other) noexcept = default;
    DynamicStateModel &operator=(const DynamicStateModel &other) noexcept = default;
    DynamicStateModel &operator=(DynamicStateModel &&other) noexcept = default;
};
