/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Framebuffer
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Framebuffer.hpp"

using namespace kF;

GPU::Framebuffer::~Framebuffer(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyFramebuffer(parent().logicalDevice(), handle(), nullptr);
}

GPU::Framebuffer::Framebuffer(const FramebufferModel &model) noexcept
{
    if (const auto res = ::vkCreateFramebuffer(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Framebuffer: Couldn't create framebuffer '", ErrorMessage(res), '\'');
}