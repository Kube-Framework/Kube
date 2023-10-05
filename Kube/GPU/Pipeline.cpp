/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#include "GPU.hpp"
#include "Pipeline.hpp"

using namespace kF;

GPU::Pipeline::~Pipeline(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyPipeline(parent().logicalDevice(), handle(), nullptr);
}

GPU::Pipeline::Pipeline(const GraphicPipelineModel &model) noexcept
{
    if (const auto res = ::vkCreateGraphicsPipelines(parent().logicalDevice(), VkPipelineCache(), 1u, &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Pipeline: Couldn't create graphic pipeline '", ErrorMessage(res), '\'');
}

GPU::Pipeline::Pipeline(const ComputePipelineModel &model) noexcept
{
    if (const auto res = ::vkCreateComputePipelines(parent().logicalDevice(), VkPipelineCache(), 1u, &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Pipeline: Couldn't create compute pipeline '", ErrorMessage(res), '\'');
}