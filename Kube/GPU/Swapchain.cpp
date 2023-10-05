/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Swapchain
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Swapchain.hpp"

using namespace kF;

GPU::Swapchain::~Swapchain(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroySwapchainKHR(parent().logicalDevice(), handle(), nullptr);
}

GPU::Swapchain::Swapchain(void) noexcept
{
    createSwapchain(NullHandle);
}

void GPU::Swapchain::createSwapchain(const SwapchainHandle oldSwapchain) noexcept
{
    const auto surfaceFormat = parent().surface().getSurfaceFormat();
    const auto presentMode = parent().surface().getPresentMode();
    const auto capabilites = parent().surface().getSurfaceCapabilities();
    const auto extent = parent().surface().getExtent(capabilites);
    const auto imageCount = [](const auto &capabilites) {
        if (capabilites.maxImageCount)
            return std::min(capabilites.maxImageCount, MaxCachedFrame);
        else
            return capabilites.minImageCount + 1;
    }(capabilites);
    const VkSwapchainCreateInfoKHR swapchainInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = VkSwapchainCreateFlagsKHR(),
        .surface = parent().surface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = capabilites.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = static_cast<VkPresentModeKHR>(presentMode),
        .clipped = true,
        .oldSwapchain = oldSwapchain
    };

    // If extent is null, we cannot create swapchain
    if (!extent.width || !extent.height) {
        if (handle() != NullHandle) {
            ::vkDestroySwapchainKHR(parent().logicalDevice(), handle(), nullptr);
            handle() = NullHandle;
        }
        _extent = {};
        _surfaceFormat = {};
        _presentMode = {};
        return;
    }

    // Create new swapchain
    if (const auto res = ::vkCreateSwapchainKHR(parent().logicalDevice(), &swapchainInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Swapchain::createSwapchain: Couldn't create swapchain '", ErrorMessage(res), '\'');
    // Destroy old swapchain
    if (oldSwapchain != NullHandle)
        ::vkDestroySwapchainKHR(parent().logicalDevice(), oldSwapchain, nullptr);
    _extent = extent;
    _surfaceFormat = surfaceFormat;
    _presentMode = presentMode;
#if KUBE_DEBUG_BUILD
    kFInfo("[GPU] Swapchain creation: PresentMode '", PresentModeName(_presentMode), "' Extent (",  _extent.width, ", ", _extent.height, ')');
#endif
}

GPU::Swapchain::SwapchainImages GPU::Swapchain::queryImages(void) const noexcept
{
    SwapchainImages images;

    if (const auto res = Internal::FillVkContainer(images, &::vkGetSwapchainImagesKHR, parent().logicalDevice(), handle()); res != VK_SUCCESS)
        kFAbort("GPU::Swapchain::queryImages: Couldn't retreive swapchain images '", ErrorMessage(res), '\'');
    return images;
}

void GPU::Swapchain::onViewSizeChanged(void) noexcept
{
    parent().logicalDevice().waitIdle();
    createSwapchain(handle());
}