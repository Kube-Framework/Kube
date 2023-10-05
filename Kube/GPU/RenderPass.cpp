/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Render Pass
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "RenderPass.hpp"

using namespace kF;

GPU::RenderPass::~RenderPass(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyRenderPass(parent().logicalDevice(), handle(), nullptr);
}

GPU::RenderPass::RenderPass(const RenderPassModel &model) noexcept
{
    if (const auto res = ::vkCreateRenderPass(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::RenderPass: Couldn't create render pass '", ErrorMessage(res), '\'');
}