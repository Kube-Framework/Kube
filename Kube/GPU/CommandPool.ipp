/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandPool
 */

#include <Kube/Core/Abort.hpp>

inline kF::GPU::CommandHandle kF::GPU::CommandPool::add(const CommandLevel level) noexcept
{
    CommandHandle command;

    add(level, &command, &command + 1);
    return command;
}

template<kF::GPU::RecorderRequirements Recorder>
inline void kF::GPU::CommandPool::recordImpl(const CommandHandle command, const CommandBufferUsageFlags usageFlags,
            const CommandInheritanceInfo * const inheritance, Recorder &&recorder) noexcept
{
    VkCommandBufferBeginInfo commandBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = ToFlags(usageFlags),
        .pInheritanceInfo = inheritance
    };

    // Begin command buffer
    if (const auto res = ::vkBeginCommandBuffer(command, &commandBeginInfo); res != VK_SUCCESS)
        kFAbort("GPU::CommandPool::add: Couldn't begin command buffer '", ErrorMessage(res), '\'');

    // Record command buffer
    recorder(command);

    // End command buffer
    if (const auto res = ::vkEndCommandBuffer(command); res != VK_SUCCESS)
        kFAbort("GPU::CommandPool::add: Couldn't end command buffer '", ErrorMessage(res), '\'');
}