/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandPool
 */

#pragma once

#include "Handle.hpp"
#include "CommandRecorder.hpp"
#include "CommandInheritanceInfo.hpp"

namespace kF::GPU
{
    class CommandPool;

    template<typename Recorder>
    concept RecorderRequirements = requires {
        std::declval<Recorder>().operator()(std::declval<CommandRecorder &>());
    };
}

/** @brief Command pool that allocates command buffers */
class kF::GPU::CommandPool : public Handle<VkCommandPool>
{
public:
    /** @brief Destruct the command pool */
    ~CommandPool(void) noexcept;

    /** @brief Construct the command pool */
    CommandPool(const QueueType queueType, const CommandPoolCreateFlags flags) noexcept;

    /** @brief Move constructor */
    CommandPool(CommandPool &&other) noexcept = default;

    /** @brief Move assignment */
    CommandPool &operator=(CommandPool &&other) noexcept = default;


    /** @brief Allocate a single command buffer */
    [[nodiscard]] CommandHandle add(const CommandLevel level) noexcept;

    /** @brief Allocate multiple command buffers */
    void add(const CommandLevel level, CommandHandle * const commandFrom, CommandHandle * const commandTo) noexcept;


    /** @brief Record a single primary command buffer */
    template<kF::GPU::RecorderRequirements Recorder>
    inline void record(const CommandHandle command, const CommandBufferUsageFlags usageFlags, Recorder &&recorder) noexcept
        { recordImpl(command, usageFlags, nullptr, std::forward<Recorder>(recorder)); }

    /** @brief Record a single secondary command buffer */
    template<kF::GPU::RecorderRequirements Recorder>
    inline void record(const CommandHandle command, const CommandBufferUsageFlags usageFlags,
            const CommandInheritanceInfo &inheritance, Recorder &&recorder) noexcept
        { recordImpl(command, usageFlags, &inheritance, std::forward<Recorder>(recorder)); }


    /** @brief Remove a command from the pool */
    inline void remove(const CommandHandle command) noexcept { remove(&command, &command + 1); }

    /** @brief Remove a range commands from the pool */
    void remove(const CommandHandle * const commandBegin, const CommandHandle * const commandEnd) noexcept;


    /** @brief Reset the command pool */
    void reset(const CommandPoolResetFlags flags = CommandPoolResetFlags::None) noexcept;

private:
    /** @brief Implementation of the record function */
    template<kF::GPU::RecorderRequirements Recorder>
    void recordImpl(const CommandHandle command, const CommandBufferUsageFlags usageFlags,
            const CommandInheritanceInfo * const inheritance, Recorder &&recorder) noexcept;
};

#include "CommandPool.ipp"