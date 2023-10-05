/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Surface
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Surface.hpp"

using namespace kF;

GPU::Surface::~Surface(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroySurfaceKHR(parent().instance(), handle(), nullptr);
}

GPU::Surface::Surface(void) noexcept
{
    if (!::SDL_Vulkan_CreateSurface(parent().backendWindow(), parent().instance(), &handle()))
        kFAbort("GPU::Surface::Surface: Couldn't create surface '", ::SDL_GetError(), '\'');
}

GPU::SurfaceFormat GPU::Surface::getSurfaceFormat(void) const noexcept
{
    Core::Vector<SurfaceFormat, GPUAllocator> formats;

    if (const auto res = Internal::FillVkContainer(formats, &::vkGetPhysicalDeviceSurfaceFormatsKHR, parent().physicalDevice(), handle()); res != VK_SUCCESS || formats.empty())
        kFAbort("GPU::Surface::surfaceFormat: Couldn't retreive physical device surface format '", ErrorMessage(res), '\'');
    for (const auto &format : formats) {
        if (format.format != VK_FORMAT_B8G8R8A8_UNORM || format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            continue;
        return format;
    }
    kFError("[GPU] Couldn't find SurfaceFormat with B8G8R8A8_UNORM and SRGB_NONLINEAR, using first avaible format");
    return formats[0];
}

GPU::PresentMode GPU::Surface::getPresentMode(void) const noexcept
{
    Core::Vector<VkPresentModeKHR, GPUAllocator> modes;

    if (const auto res = Internal::FillVkContainer(modes, &::vkGetPhysicalDeviceSurfacePresentModesKHR, parent().physicalDevice(), handle()); res != VK_SUCCESS)
        kFAbort("GPU::Surface::getPresentMode: Couldn't retreive physical device present modes");
    for (const auto &mode : modes) {
        if (static_cast<PresentMode>(mode) == PresentMode::MailboxKhr)
            return static_cast<PresentMode>(mode);
    }
    for (const auto &mode : modes) {
        if (static_cast<PresentMode>(mode) == PresentMode::FifoKhr) {
            kFError("[GPU] PresentMode::MailboxKhr is not available, using PresentMode::FifoKhr");
                return static_cast<PresentMode>(mode);
        }
    }
    kFError("[GPU] Neither PresentMode::MailboxKhr nor PresentMode::FifoKhr are available, using PresentMode::Immediate");
    return PresentMode::ImmediateKhr;
}

GPU::SurfaceCapabilities GPU::Surface::getSurfaceCapabilities(void) const noexcept
{
    SurfaceCapabilities capabilities {};

    if (const auto res = ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(parent().physicalDevice(), handle(), &capabilities); res != VK_SUCCESS)
        kFAbort("GPU::Surface::surfaceCapabilities: Couldn't retreive physical device surface capabilities '", ErrorMessage(res), '\'');
    return capabilities;
}

GPU::Extent2D GPU::Surface::getExtent(const SurfaceCapabilities &capabilities) const noexcept
{
    int width = 0, height = 0;

    if (capabilities.currentExtent.width != UINT32_MAX && capabilities.currentExtent.height != UINT32_MAX)
        return capabilities.currentExtent;
    ::SDL_Vulkan_GetDrawableSize(parent().backendWindow(), &width, &height);
    return Extent2D {
        .width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, static_cast<std::uint32_t>(width))),
        .height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, static_cast<std::uint32_t>(height))),
    };
}