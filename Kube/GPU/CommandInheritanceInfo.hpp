/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandInheritanceInfo
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct CommandInheritanceInfo;
};

/** @brief Describe how to create a color blend */
struct kF::GPU::CommandInheritanceInfo : public VkCommandBufferInheritanceInfo
{
    /** @brief Destructor */
    ~CommandInheritanceInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    CommandInheritanceInfo(
        const RenderPassHandle renderPass_ = NullHandle,
        const std::uint32_t subpass_ = 0u,
        const FramebufferHandle framebuffer_ = NullHandle,
        const bool occlusionQueryEnable_ = false,
        const QueryControlFlags queryFlags_ = QueryControlFlags::None,
        const QueryPipelineStatisticFlags pipelineStatistics_ = QueryPipelineStatisticFlags::None) noexcept
        : VkCommandBufferInheritanceInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .pNext = nullptr,
            .renderPass = renderPass_,
            .subpass = subpass_,
            .framebuffer = framebuffer_,
            .occlusionQueryEnable = occlusionQueryEnable_,
            .queryFlags = static_cast<VkQueryControlFlags>(queryFlags_),
            .pipelineStatistics = static_cast<VkQueryPipelineStatisticFlags>(pipelineStatistics_)
        } {}


    /** @brief POD semantics */
    CommandInheritanceInfo(const CommandInheritanceInfo &other) noexcept = default;
    CommandInheritanceInfo(CommandInheritanceInfo &&other) noexcept = default;
    CommandInheritanceInfo &operator=(const CommandInheritanceInfo &other) noexcept = default;
    CommandInheritanceInfo &operator=(CommandInheritanceInfo &&other) noexcept = default;
};
