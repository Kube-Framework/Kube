/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: CommandRecording
 */

#pragma once

#include "Base.hpp"
#include "RenderPassBeginInfo.hpp"
#include "BufferCopy.hpp"
#include "BufferImageCopy.hpp"
#include "MemoryBarrier.hpp"

namespace kF::GPU
{
    class CommandRecorder;
}

/** @brief This recorder allow to record command buffers */
class kF::GPU::CommandRecorder
{
public:
    /** @brief Destructor */
    ~CommandRecorder(void) noexcept = default;

    /** @brief Construct the recorder out of a command (has no ownership) */
    CommandRecorder(const CommandHandle command) noexcept : _command(command) {}

    /** POD semantics */
    CommandRecorder(const CommandRecorder &other) noexcept = default;
    CommandRecorder(CommandRecorder &&other) noexcept = default;
    CommandRecorder &operator=(const CommandRecorder &other) noexcept = default;
    CommandRecorder &operator=(CommandRecorder &&other) noexcept = default;


    /** @brief Execute a single secondary command buffer */
    inline void executeCommand(const CommandHandle commandHandle) const noexcept
        { ::vkCmdExecuteCommands(_command, 1, &commandHandle); }

    /** @brief Execute secondary command buffers */
    inline void executeCommands(const CommandHandle * const commandBegin, const CommandHandle * const commandEnd) const noexcept
        { ::vkCmdExecuteCommands(_command, Core::Distance<std::uint32_t>(commandBegin, commandEnd), commandBegin); }


    /** @brief Begin a render pass */
    inline void beginRenderPass(const RenderPassBeginInfo &renderPassBegin, const SubpassContents contents) const noexcept
        { ::vkCmdBeginRenderPass(_command, &renderPassBegin, static_cast<VkSubpassContents>(contents)); }

    /** @brief Begin a render pass (inline arguments) */
    void beginRenderPass(const RenderPassHandle renderPass, const FramebufferHandle framebuffer,
            const Rect2D renderArea, const std::initializer_list<ClearValue> &clears, const SubpassContents contents) const noexcept;


    /** @brief End a render pass */
    inline void endRenderPass(void) const noexcept
        { ::vkCmdEndRenderPass(_command); }

    /** @brief Proceed to the next sub pass */
    inline void nextSubpass(const SubpassContents subpassContents) const noexcept
        { ::vkCmdNextSubpass(_command, static_cast<VkSubpassContents>(subpassContents)); }


    /** @brief Bind a pipeline */
    inline void bindPipeline(const PipelineBindPoint pipelineBindPoint, const PipelineHandle pipeline) const noexcept
        { ::vkCmdBindPipeline(_command, static_cast<VkPipelineBindPoint>(pipelineBindPoint), pipeline); }


    /** @brief Bind descriptor sets */
    inline void bindDescriptorSets(
        const PipelineBindPoint pipelineBindPoint,
        const PipelineLayoutHandle pipelineLayout,
        const std::uint32_t firstSet,
        const DescriptorSetHandle * const descriptorSetBegin,
        const DescriptorSetHandle * const descriptorSetEnd,
        const std::uint32_t * const dynamicOffsetBegin = nullptr,
        const std::uint32_t * const dynamicOffsetEnd = nullptr
    ) const noexcept;

    /** @brief Bind a single descriptor set */
    inline void bindDescriptorSet(
        const PipelineBindPoint pipelineBindPoint,
        const PipelineLayoutHandle pipelineLayout,
        const std::uint32_t firstSet,
        const DescriptorSetHandle descriptorSet,
        const std::uint32_t * const dynamicOffsetBegin = nullptr,
        const std::uint32_t * const dynamicOffsetEnd = nullptr
    ) const noexcept
        { bindDescriptorSets(pipelineBindPoint, pipelineLayout, firstSet, &descriptorSet, &descriptorSet + 1, dynamicOffsetBegin, dynamicOffsetEnd); }


    /** @brief Execute a single secondary command buffer */
    template<typename Data>
        requires std::is_trivially_copyable_v<Data>
    inline void pushConstants(
        const PipelineLayoutHandle pipelineLayout,
        const ShaderStageFlags shaderStageFlags,
        const Data &data,
        const std::uint32_t offset = {}
    ) const noexcept
        { ::vkCmdPushConstants(_command, pipelineLayout, static_cast<VkShaderStageFlags>(shaderStageFlags), offset, sizeof(Data), &data); }

    /** @brief Draw commands */
    inline void draw(
        const std::uint32_t vertexCount,
        const std::uint32_t instanceCount = 1,
        const std::uint32_t firstVertex = 0u,
        const std::uint32_t firstInstance = 0
    ) const noexcept
        { ::vkCmdDraw(_command, vertexCount, instanceCount, firstVertex, firstInstance); }

    /** @brief Indexed draw commands */
    inline void drawIndexed(
        const std::uint32_t indexCount,
        const std::uint32_t instanceCount = 1,
        const std::uint32_t firstIndex = 0,
        const std::int32_t vertexOffset = 0,
        const std::uint32_t firstInstance = 0u
    ) const noexcept
        { ::vkCmdDrawIndexed(_command, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance); }


    /** @brief Indirect draw commands */
    inline void drawIndirect(
        const BufferHandle buffer,
        const BufferSize offset,
        const std::uint32_t drawCount,
        const std::uint32_t stride
    ) const noexcept
        { ::vkCmdDrawIndirect(_command, buffer, offset, drawCount, stride); }

    /** @brief Indexed indirect draw commands */
    inline void drawIndexedIndirect(const BufferHandle buffer, const BufferSize bufferOffset, const std::uint32_t drawCount, const std::uint32_t stride) const noexcept
        { ::vkCmdDrawIndexedIndirect(_command, buffer, bufferOffset, drawCount, stride); }


    /** @brief Indirect draw commands with indirect draw count */
    inline void drawIndirectCount(
        const BufferHandle buffer,
        const BufferSize bufferOffset,
        const BufferHandle countBuffer,
        const BufferSize countBufferOffset,
        const std::uint32_t maxDrawCount,
        const std::uint32_t stride
    ) const noexcept
        { ::vkCmdDrawIndirectCount(_command, buffer, bufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride); }

    /** @brief Indexed indirect draw commands with indirect draw count */
    inline void drawIndexedIndirectCount(
        const BufferHandle buffer,
        const BufferSize bufferOffset,
        const BufferHandle countBuffer,
        const BufferSize countBufferOffset,
        const std::uint32_t maxDrawCount,
        const std::uint32_t stride
    ) const noexcept
        { ::vkCmdDrawIndexedIndirectCount(_command, buffer, bufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride); }


    /** @brief Set dynamic viewport
     *  @note Graphic pipeline must be created with DynamicStage::Viewport */
    inline void setViewport(const Viewport &viewport) const noexcept
        { ::vkCmdSetViewport(_command, 0, 1, &viewport); }


    /** @brief Set dynamic scissor
     *  @note Graphic pipeline must be created with DynamicStage::Scissor */
    inline void setScissor(const Rect2D &scissor) const noexcept
        { ::vkCmdSetScissor(_command, 0, 1, &scissor); }


    /** @brief Bind a single vertex buffer */
    inline void bindVertexBuffer(
        const std::uint32_t binding,
        const BufferHandle buffer,
        const BufferSize offset = 0u
    ) const noexcept
        { ::vkCmdBindVertexBuffers(_command, binding, 1, &buffer, &offset); }

    /** @brief Bind vertex buffers */
    inline void bindVertexBuffers(
        const std::uint32_t firstBinding,
        const std::uint32_t bindingCount,
        const BufferHandle * const pBuffers,
        const BufferSize * const pOffsets
    ) const noexcept
        { ::vkCmdBindVertexBuffers(_command, firstBinding, bindingCount, pBuffers, pOffsets); }


    /** @brief Bind a single index buffer */
    inline void bindIndexBuffer(
        const BufferHandle buffer,
        const IndexType indexType,
        const BufferSize offset = 0u
    ) const noexcept
        { ::vkCmdBindIndexBuffer(_command, buffer, offset, static_cast<VkIndexType>(indexType)); }


    /** @brief Buffer copy command with one region */
    inline void copyBuffer(
        const BufferHandle srcBuffer,
        const BufferHandle dstBuffer,
        const BufferCopy &region
    ) const noexcept
        { ::vkCmdCopyBuffer(_command, srcBuffer, dstBuffer, 1, &region); }

    /** @brief Buffer copy command with multiple regions */
    inline void copyBuffer(
        const BufferHandle srcBuffer,
        const BufferHandle dstBuffer,
        const BufferCopy * const regionBegin,
        const BufferCopy * const regionEnd
    ) const noexcept
        { ::vkCmdCopyBuffer(_command, srcBuffer, dstBuffer, Core::Distance<std::uint32_t>(regionBegin, regionEnd), regionBegin); }


    /** @brief Buffer to image copy command with one region */
    inline void copyBufferToImage(
        const BufferHandle srcBuffer,
        const ImageHandle dstImage,
        const ImageLayout dstImageLayout,
        const BufferImageCopy &region
    ) const noexcept
        { ::vkCmdCopyBufferToImage(_command, srcBuffer, dstImage, static_cast<VkImageLayout>(dstImageLayout), 1, &region); }

    /** @brief Buffer to image copy command with multiple regions */
    inline void copyBufferToImage(
        const BufferHandle srcBuffer,
        const ImageHandle dstImage,
        const ImageLayout dstImageLayout,
        const BufferImageCopy * const regionBegin,
        const BufferImageCopy * const regionEnd
    ) const noexcept
        { ::vkCmdCopyBufferToImage(_command, srcBuffer, dstImage, static_cast<VkImageLayout>(dstImageLayout), Core::Distance<std::uint32_t>(regionBegin, regionEnd), regionBegin); }


    /** @brief Dispatch a compute shader command */
    inline void dispatch(
        const std::uint32_t groupCountX,
        const std::uint32_t groupCountY = 1u,
        const std::uint32_t groupCountZ = 1u
    ) const noexcept
        { ::vkCmdDispatch(_command, groupCountX, groupCountY, groupCountZ); }

    /** @brief Dispatch a compute shader command using group bases */
    inline void dispatchBase(
        const std::uint32_t groupBaseX,
        const std::uint32_t groupCountX,
        const std::uint32_t groupBaseY = 0u,
        const std::uint32_t groupCountY = 1u,
        const std::uint32_t groupBaseZ = 0u,
        const std::uint32_t groupCountZ = 1u
    ) const noexcept
        { ::vkCmdDispatchBase(_command, groupBaseX, groupBaseY, groupBaseZ, groupCountX, groupCountY, groupCountZ); }


    /** @brief Pipeline barriers command */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const MemoryBarrier * const memoryBarrierBegin,
        const MemoryBarrier * const memoryBarrierEnd,
        const BufferMemoryBarrier * const bufferBarrierBegin,
        const BufferMemoryBarrier * const bufferBarrierEnd,
        const ImageMemoryBarrier * const imageBarrierBegin,
        const ImageMemoryBarrier * const imageBarrierEnd
    ) const noexcept;

    /** @brief Pipeline barriers command without specialization */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags = DependencyFlags::None
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr); }

    /** @brief Pipeline barriers command only using memory barriers */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const MemoryBarrier * const memoryBarrierBegin,
        const MemoryBarrier * const memoryBarrierEnd
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, memoryBarrierBegin, memoryBarrierEnd, nullptr, nullptr, nullptr, nullptr); }

    /** @brief Pipeline barriers command only using a single memory barrier */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const MemoryBarrier &memoryBarrier
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, &memoryBarrier, &memoryBarrier + 1, nullptr, nullptr, nullptr, nullptr); }

    /** @brief Pipeline barriers command only using buffer memory barriers */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const BufferMemoryBarrier * const bufferBarrierBegin,
        const BufferMemoryBarrier * const bufferBarrierEnd
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, nullptr, nullptr, bufferBarrierBegin, bufferBarrierEnd, nullptr, nullptr); }

    /** @brief Pipeline barriers command only using a single buffer memory barrier */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const BufferMemoryBarrier &bufferBarrier
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, nullptr, nullptr, &bufferBarrier, &bufferBarrier + 1, nullptr, nullptr); }

    /** @brief Pipeline barriers command only using image memory barriers */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const ImageMemoryBarrier * const imageBarrierBegin,
        const ImageMemoryBarrier * const imageBarrierEnd
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, nullptr, nullptr, nullptr, nullptr, imageBarrierBegin, imageBarrierEnd); }

    /** @brief Pipeline barriers command only using a single image memory barrier */
    inline void pipelineBarrier(
        const PipelineStageFlags sourceStage,
        const PipelineStageFlags destinationStage,
        const DependencyFlags dependencyFlags,
        const ImageMemoryBarrier &imageBarrier
    ) const noexcept
        { pipelineBarrier(sourceStage, destinationStage, dependencyFlags, nullptr, nullptr, nullptr, nullptr, &imageBarrier, &imageBarrier + 1); }

private:
    CommandHandle _command;
};

#include "CommandRecorder.ipp"