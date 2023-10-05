/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PipelineLayout
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "PipelineLayout.hpp"

using namespace kF;

GPU::PipelineLayout::~PipelineLayout(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyPipelineLayout(parent().logicalDevice(), handle(), nullptr);
}

GPU::PipelineLayout::PipelineLayout(const PipelineLayoutModel &model) noexcept
{
    if (const auto res = ::vkCreatePipelineLayout(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::PipelineLayout: Couldn't create pipeline layout '", ErrorMessage(res), '\'');
}