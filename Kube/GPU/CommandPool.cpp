/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandPool
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "CommandPool.hpp"

using namespace kF;

GPU::CommandPool::~CommandPool(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyCommandPool(parent().logicalDevice(), handle(), nullptr);
}

GPU::CommandPool::CommandPool(const QueueType queueType, const CommandPoolCreateFlags flags) noexcept
{
    VkCommandPoolCreateInfo commandPoolInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = ToFlags(flags),
        .queueFamilyIndex = parent().queueManager().queueDescriptor(queueType).queueFamilyIndex
    };

    if (const auto res = ::vkCreateCommandPool(parent().logicalDevice(), &commandPoolInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::CommandPool: Couldn't create command pool '", ErrorMessage(res), '\'');
}

void GPU::CommandPool::add(const CommandLevel level, CommandHandle * const commandFrom, CommandHandle * const commandTo) noexcept
{
    VkCommandBufferAllocateInfo commandInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = handle(),
        .level = static_cast<VkCommandBufferLevel>(level),
        .commandBufferCount = Core::Distance<std::uint32_t>(commandFrom, commandTo)
    };

    if (const auto res = ::vkAllocateCommandBuffers(parent().logicalDevice(), &commandInfo, commandFrom); res != VK_SUCCESS)
        kFAbort("GPU::CommandPool::allocateCommands: Couldn't allocate command buffers '", ErrorMessage(res), '\'');
}

void GPU::CommandPool::remove(const CommandHandle * const commandBegin, const CommandHandle * const commandEnd) noexcept
{
    ::vkFreeCommandBuffers(
        parent().logicalDevice(), handle(),
        Core::Distance<std::uint32_t>(commandBegin, commandEnd), commandBegin
    );
}

void GPU::CommandPool::reset(const CommandPoolResetFlags flags) noexcept
{
    ::vkResetCommandPool(parent().logicalDevice(), handle(), ToFlags(flags));
}