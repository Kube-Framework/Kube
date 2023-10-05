/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Fence
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Fence.hpp"

using namespace kF;

bool GPU::Fence::Wait(const FenceHandle * const begin, const FenceHandle * const end,
        const bool waitAll, const std::uint64_t timeout) noexcept
{
    if (const auto res = ::vkWaitForFences(Parent().logicalDevice(), Core::Distance<std::uint32_t>(begin, end), begin, waitAll, timeout); res == VK_TIMEOUT)
        return false;
    else if (res != VK_SUCCESS)
        kFAbort("GPU::Fence::Wait: Error on waiting fence(s) '", ErrorMessage(res), '\'');
    return true;
}

void GPU::Fence::Reset(const FenceHandle * const begin, const FenceHandle * const end) noexcept
{
    ::vkResetFences(Parent().logicalDevice(), Core::Distance<std::uint32_t>(begin, end), begin);
}

GPU::Fence::~Fence(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyFence(parent().logicalDevice(), handle(), nullptr);
}

GPU::Fence::Fence(const bool signaled) noexcept
{
    static_assert(VK_FENCE_CREATE_SIGNALED_BIT == true, "Vulkan flag VK_FENCE_CREATE_SIGNALED_BIT is not 1, please review this function since it may be unsafe");

    VkFenceCreateInfo fenceInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = signaled
    };

    if (const auto res = ::vkCreateFence(parent().logicalDevice(), &fenceInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Fence: Couldn't create fence '", ErrorMessage(res), '\'');
}

bool GPU::Fence::isSignaled(void) const noexcept
{
    return ::vkGetFenceStatus(parent().logicalDevice(), handle()) == VK_SUCCESS;
}