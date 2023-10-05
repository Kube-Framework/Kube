/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GraphicPipelineModel
 */

#pragma once

#include "ShaderStageModel.hpp"
#include "VertexInputModel.hpp"
#include "InputAssemblyModel.hpp"
#include "TessellationModel.hpp"
#include "ViewportModel.hpp"
#include "RasterizationModel.hpp"
#include "MultisampleModel.hpp"
#include "DepthStencilModel.hpp"
#include "ColorBlendModel.hpp"
#include "DynamicStateModel.hpp"

namespace kF::GPU
{
    struct GraphicPipelineModel;
};

/** @brief Describe how to create a graphic pipeline */
struct kF::GPU::GraphicPipelineModel : public VkGraphicsPipelineCreateInfo
{
    /** @brief Destructor */
    ~GraphicPipelineModel(void) noexcept = default;

    /** @brief Initialize constructor */
    GraphicPipelineModel(const PipelineCreateFlags flags_,
            const ShaderStageModel * const stageBegin, const ShaderStageModel * const stageEnd,
            const VertexInputModel &vertexInputModel,
            const InputAssemblyModel &inputAssemblyModel,
            const TessellationModel &tessellationModel,
            const ViewportModel &viewportModel,
            const RasterizationModel &rasterizationModel,
            const MultisampleModel &multisampleModel,
            const DepthStencilModel &depthStencilModel,
            const ColorBlendModel &colorBlendModel,
            const DynamicStateModel &dynamicModel,
            const PipelineLayoutHandle layout_,
            const RenderPassHandle renderPass_,
            const std::uint32_t subpass_,
            const PipelineHandle basePipelineHandle_ = NullHandle,
            const std::int32_t basePipelineIndex_ = 0u) noexcept
        : VkGraphicsPipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .stageCount = Core::Distance<std::uint32_t>(stageBegin, stageEnd),
            .pStages = stageBegin,
            .pVertexInputState = &vertexInputModel,
            .pInputAssemblyState = &inputAssemblyModel,
            .pTessellationState = &tessellationModel,
            .pViewportState = &viewportModel,
            .pRasterizationState = &rasterizationModel,
            .pMultisampleState = &multisampleModel,
            .pDepthStencilState = &depthStencilModel,
            .pColorBlendState = &colorBlendModel,
            .pDynamicState = &dynamicModel,
            .layout = layout_,
            .renderPass = renderPass_,
            .subpass = subpass_,
            .basePipelineHandle = basePipelineHandle_,
            .basePipelineIndex = basePipelineIndex_
        } {}

    /** @brief POD semantics */
    GraphicPipelineModel(const GraphicPipelineModel &other) noexcept = default;
    GraphicPipelineModel(GraphicPipelineModel &&other) noexcept = default;
    GraphicPipelineModel &operator=(const GraphicPipelineModel &other) noexcept = default;
    GraphicPipelineModel &operator=(GraphicPipelineModel &&other) noexcept = default;
};
