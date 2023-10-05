/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SubmitInfo
 */

#pragma once

#include <Kube/Core/Assert.hpp>

#include "Base.hpp"

namespace kF::GPU
{
    struct SubmitInfo;
};

/** @brief Describe how to create submit info command */
struct kF::GPU::SubmitInfo : public VkSubmitInfo
{
    /** @brief Destructor */
    ~SubmitInfo(void) noexcept = default;

    /** @brief Initialize constructor */
    SubmitInfo(
        const CommandHandle * const commandBegin,
        const CommandHandle * const commandEnd,
        const SemaphoreHandle * const waitBegin,
        const SemaphoreHandle * const waitEnd,
        const PipelineStageFlags * const waitStageBegin,
        [[maybe_unused]] const PipelineStageFlags * const waitStageEnd,
        const SemaphoreHandle * const signalBegin = nullptr,
        const SemaphoreHandle * const signalEnd = nullptr
    ) noexcept
        : VkSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = Core::Distance<std::uint32_t>(waitBegin, waitEnd),
            .pWaitSemaphores = waitBegin,
            .pWaitDstStageMask = reinterpret_cast<const VkPipelineStageFlags *>(waitStageBegin),
            .commandBufferCount = Core::Distance<std::uint32_t>(commandBegin, commandEnd),
            .pCommandBuffers = commandBegin,
            .signalSemaphoreCount = Core::Distance<std::uint32_t>(signalBegin, signalEnd),
            .pSignalSemaphores = signalBegin
        }
    {
        kFAssert(std::distance(waitBegin, waitEnd) == std::distance(waitStageBegin, waitStageEnd),
            "GPU::SubmitInfo::SubmitInfo: Mismatching count of wait semaphore and wait pipeline stages");
    }


    /** @brief POD semantics */
    SubmitInfo(const SubmitInfo &other) noexcept = default;
    SubmitInfo(SubmitInfo &&other) noexcept = default;
    SubmitInfo &operator=(const SubmitInfo &other) noexcept = default;
    SubmitInfo &operator=(SubmitInfo &&other) noexcept = default;
};
