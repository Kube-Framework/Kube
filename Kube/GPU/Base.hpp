/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPU Base
 */

#pragma once

#include <string_view>

#include <vulkan/vulkan.h>

#include <Kube/Core/StaticSafeAllocator.hpp>

#include "VMA.hpp"

struct SDL_Window;

// General declarations
namespace kF::GPU
{
    /** @brief Command pool manager allocator */
    struct GPUAllocator : Core::StaticSafeAllocator<"GPUAllocator"> {};

    /** @brief Backend window structure forward declaration */
    using BackendWindow = SDL_Window;


    /** @brief Null handle alias */
    constexpr auto NullHandle = nullptr;

    /** @brief Vulkan external Subpass special value */
    constexpr auto ExternalSubpassIndex = VK_SUBPASS_EXTERNAL;

    /** @brief Vulkan external Subpass special value */
    constexpr auto IgnoredFamilyQueue = VK_QUEUE_FAMILY_IGNORED;


    /** @brief Index of a frame */
    using FrameIndex = std::uint32_t;


    /** @brief Clear value */
    using ClearValue = VkClearValue;

    /** @brief Clear color value */
    using ClearColorValue = VkClearColorValue;

    /** @brief Depth stencil clear */
    using ClearDepthStencilValue = VkClearDepthStencilValue;


    /** @brief Instance handle */
    using InstanceHandle = VkInstance;


    /** @brief Physical device handle */
    using PhysicalDeviceHandle = VkPhysicalDevice;

    /** @brief Describes the properties of a physical device */
    using PhysicalDeviceProperties = VkPhysicalDeviceProperties;

    /** @brief Describes the limits of a physical device */
    using PhysicalDeviceLimits = VkPhysicalDeviceLimits;

    /** @brief Describes the features of physical device indexing extension */
    using PhysicalDeviceDescriptorIndexingFeatures = VkPhysicalDeviceDescriptorIndexingFeatures;

    /** @brief Physical device features */
    using PhysicalDeviceFeatures = VkPhysicalDeviceFeatures;

    /** @brief Features of the physical device (extended version) */
    using PhysicalDeviceFeatures2 = VkPhysicalDeviceFeatures2;


    /** @brief Logical device handle */
    using LogicalDeviceHandle = VkDevice;


    /** @brief 2D extent */
    using Extent2D = VkExtent2D;

    /** @brief 3D extent */
    using Extent3D = VkExtent3D;

    /** @brief 2D rectangle */
    using Rect2D = VkRect2D;

    /** @brief 2D offset */
    using Offset2D = VkOffset2D;

    /** @brief 3D offset */
    using Offset3D = VkOffset3D;


    /** @brief Swapchain handle */
    using SwapchainHandle = VkSwapchainKHR;


    /** @brief Surface handle */
    using SurfaceHandle = VkSurfaceKHR;

    /** @brief Vulkan surface format */
    using SurfaceFormat = VkSurfaceFormatKHR;

    /** @brief Vulkan presentation mode*/
    using SurfaceCapabilities = VkSurfaceCapabilitiesKHR;


    /** @brief Fence handle */
    using FenceHandle = VkFence;

    /** @brief Fence handle */
    using SemaphoreHandle = VkSemaphore;

    /** @brief Event handle */
    using EventHandle = VkEvent;


    /** @brief A device pipeline */
    using PipelineHandle = VkPipeline;


    /** @brief Pipeline layout handle */
    using PipelineLayoutHandle = VkPipelineLayout;

    /** @brief Descriptor set layout handle */
    using DescriptorSetLayoutHandle = VkDescriptorSetLayout;

    /** @brief Descriptor set handle */
    using DescriptorSetHandle = VkDescriptorSet;


    /** @brief Render pass handle */
    using RenderPassHandle = VkRenderPass;


    /** @brief A frame buffer handle */
    using FramebufferHandle = VkFramebuffer;


    /** @brief A vulkan queue */
    using QueueHandle = VkQueue;


    /** @brief Command handle */
    using CommandHandle = VkCommandBuffer;

    /** @brief Describe a secondary command buffer inheritance */
    using CommandBufferInheritanceInfo = VkCommandBufferInheritanceInfo;


    /** @brief Device memory allocation */
    using MemoryAllocationHandle = VmaAllocation;


    /** @brief Viewport */
    using Viewport = VkViewport;


    /** @brief Vulkan buffer */
    using BufferHandle = VkBuffer;

    /** @brief 64bits size of a buffer */
    using BufferSize = VkDeviceSize;


    /** @brief Buffer view handle */
    using BufferViewHandle = VkBufferView;


    /** @brief Image handle */
    using ImageHandle = VkImage;

    /** @brief Image view handle */
    using ImageViewHandle = VkImageView;


    /** @brief Sampler handle */
    using SamplerHandle = VkSampler;


    /** @brief Shader module */
    using ShaderModuleHandle = VkShaderModule;


    /** @brief Sample mask */
    using SampleMask = VkSampleMask;
}


// Flags declarations
namespace kF::GPU
{
    /** @brief Base of any flag */
    using Flags = VkFlags;

    /** @brief Helper used to cast enum class to flags */
    template<typename Type> requires requires { static_cast<Flags>(std::declval<Type>()); }
    [[nodiscard]] constexpr Flags ToFlags(const Type value) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        static_assert(std::is_same_v<std::underlying_type_t<Type>, Flags>);
        return static_cast<Underlying>(value);
    }

    /** @brief Queue flags */
    enum class QueueFlags : VkQueueFlags
    {
        None = 0x0,
        Graphics = VK_QUEUE_GRAPHICS_BIT,
        Compute = VK_QUEUE_COMPUTE_BIT,
        Transfer = VK_QUEUE_TRANSFER_BIT,
        SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,
        Protected = VK_QUEUE_PROTECTED_BIT
    };

    /** @brief Pipeline stage flags */
    enum class PipelineStageFlags : VkPipelineStageFlags
    {
        None = 0x0,
        TopOfPipe = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        DrawIndirect = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        VertexInput = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
        VertexShader = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
        TessellationControlShader = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
        TessellationEvaluationShader = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
        GeometryShader = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
        FragmentShader = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        EarlyFragmentTests = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        LateFragmentTests = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        ColorAttachmentOutput = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        ComputeShader = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        Transfer = VK_PIPELINE_STAGE_TRANSFER_BIT,
        BottomOfPipe = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        Host = VK_PIPELINE_STAGE_HOST_BIT,
        AllGraphics = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
        AllCommands = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
    };


    /** @brief Attachment description flags */
    enum class AttachmentDescriptionFlags : VkAttachmentDescriptionFlags
    {
        None = 0x0,
        MayAlias = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
    };


    /** @brief Pipeline layout creation flags */
    enum class PipelineLayoutCreateFlags : VkPipelineLayoutCreateFlags
    {
        None = 0x0
    };

    /** @brief Format feature flags */
    enum class FormatFeatureFlags : VkFormatFeatureFlags
    {
        SampledImage = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT,
        StorageImage = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
        StorageImageAtomic = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT,
        UniformTexelBuffer = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT,
        StorageTexelBuffer = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT,
        StorageTexelBufferAtomic = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT,
        VertexBuffer = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT,
        ColorAttachment = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,
        ColorAttachmentBlend = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT,
        DepthStencilAttachment = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
        BlitSrc = VK_FORMAT_FEATURE_BLIT_SRC_BIT,
        BlitDst = VK_FORMAT_FEATURE_BLIT_DST_BIT,
        SampledImageFilterLinear = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT,
        TransferSrc = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT,
        TransferDst = VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
        MidpointChromaSamples = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT,
        SampledImageYcbcrConversionLinearFilter = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT,
        SampledImageYcbcrConversionSeparateReconstructionFilter = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT,
        SampledImageYcbcrConversionChromaReconstructionExplicit = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT,
        SampledImageYcbcrConversionChromaReconstructionExplicitForceable = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT,
        Disjoint = VK_FORMAT_FEATURE_DISJOINT_BIT,
        CositedChromaSamples = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT,
        SampledImageFilterMinmax = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT,
        SampledImageFilterCubicBit = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG
    };

    /** @brief Frame buffer creation flags */
    enum class FramebufferCreateFlags : VkFramebufferCreateFlags
    {
        None = 0x0,
        Imageless = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT
    };


    /** @brief Buffer creation flags */
    enum class BufferCreateFlags : VkBufferCreateFlags
    {
        None = 0x0,
        SparseBinding = VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
        SparseResidency = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT,
        SparseAliased = VK_BUFFER_CREATE_SPARSE_ALIASED_BIT,
        Protected = VK_BUFFER_CREATE_PROTECTED_BIT,
        DeviceAddressCaptureReplay = VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT
    };

    /** @brief Buffer usage flags */
    enum class BufferUsageFlags : VkBufferUsageFlags
    {
        None = 0x0,
        TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
        StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
        UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
        ShaderDeviceAddress = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    };


    /** @brief Image aspect flags */
    enum class ImageAspectFlags : VkImageAspectFlags
    {
        Color = VK_IMAGE_ASPECT_COLOR_BIT,
        Depth = VK_IMAGE_ASPECT_DEPTH_BIT,
        Stencil = VK_IMAGE_ASPECT_STENCIL_BIT,
        Metadata = VK_IMAGE_ASPECT_METADATA_BIT,
        Plane_0 = VK_IMAGE_ASPECT_PLANE_0_BIT,
        Plane_1 = VK_IMAGE_ASPECT_PLANE_1_BIT,
        Plane_2 = VK_IMAGE_ASPECT_PLANE_2_BIT
    };

    /** @brief Image creation flags */
    enum class ImageCreateFlags : VkImageCreateFlags
    {
        None = 0x0,
        SparseBinding = VK_IMAGE_CREATE_SPARSE_BINDING_BIT,
        SparseResidency = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT,
        SparseAliased = VK_IMAGE_CREATE_SPARSE_ALIASED_BIT,
        MutableFormat = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
        CubeCompatible = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        Alias = VK_IMAGE_CREATE_ALIAS_BIT,
        SplitInstanceBindRegions = VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT,
        Array2DCompatible = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
        BlockTexelViewCompatible = VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
        ExtendedUsage = VK_IMAGE_CREATE_EXTENDED_USAGE_BIT,
        Protected = VK_IMAGE_CREATE_PROTECTED_BIT,
        Disjoint = VK_IMAGE_CREATE_DISJOINT_BIT
    };

    /** @brief Image usage flags */
    enum class ImageUsageFlags : VkImageUsageFlags
    {
        None = 0x0,
        TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,
        Storage = VK_IMAGE_USAGE_STORAGE_BIT,
        ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        DepthStencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        TransientAttachment = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        InputAttachment = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
    };


    /** @brief Image view creation flags */
    enum class ImageViewCreateFlags : VkImageViewCreateFlags
    {
        None = 0x0,
        Dynamic = VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT
    };

    /** @brief Sampler creation flags */
    enum class SamplerCreateFlags : VkSamplerCreateFlags
    {
        None = 0x0,
        SubsampledExt = VK_SAMPLER_CREATE_SUBSAMPLED_BIT_EXT,
        SubsampledCoarseReconstructionExt = VK_SAMPLER_CREATE_SUBSAMPLED_COARSE_RECONSTRUCTION_BIT_EXT
    };


    /** @brief Command buffer usage flags */
    enum class CommandBufferUsageFlags : VkCommandBufferUsageFlags
    {
        None = 0x0,
        OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        RenderPassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
        SimultaneousUse = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
    };


    /** @brief Query control flags */
    enum class QueryControlFlags : VkQueryControlFlags
    {
        None = 0x0,
        Precise = VK_QUERY_CONTROL_PRECISE_BIT
    };

    /** @brief Query pipeline statistics flags */
    enum class QueryPipelineStatisticFlags : VkQueryPipelineStatisticFlags
    {
        None = 0x0,
        InputAssemblyVertices = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT,
        InputAssemblyPrimitives = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT,
        VertexShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT,
        GeometryShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT,
        GeometryShaderPrimitives = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT,
        ClippingInvocations = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT,
        ClippingPrimitives = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT,
        FragmentShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT,
        TessellationControlShaderPatches = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT,
        TessellationEvaluationShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT,
        ComputeShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
    };


    /** @brief Pipeline creation flags */
    enum class PipelineCreateFlags : VkPipelineCreateFlags
    {
        None = 0x0,
        DisableOptimization = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT,
        AllowDerivatives = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,
        Derivative = VK_PIPELINE_CREATE_DERIVATIVE_BIT,
        ViewIndexFromDeviceIndex = VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT,
        DispatchBase = VK_PIPELINE_CREATE_DISPATCH_BASE_BIT
    };

    /** @brief Vertex input state creation flags */
    enum class VertexInputCreateFlags : VkPipelineVertexInputStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Shader stage creation flags */
    enum class ShaderStageCreateFlags : VkPipelineShaderStageCreateFlags
    {
        None = 0x0
    };

    /** @brief Shader stage flags */
    enum class ShaderStageFlags : VkShaderStageFlags
    {
        None = 0x0,
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        Compute = VK_SHADER_STAGE_COMPUTE_BIT,
        AllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS,
        All = VK_SHADER_STAGE_ALL
    };


    /** @brief Input assembly creation flags */
    enum class InputAssemblyCreateFlags : VkPipelineInputAssemblyStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Tessellation state assembly creation flags */
    enum class TessellationCreateFlags : VkPipelineTessellationStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Viewport creation flags */
    enum class ViewportCreateFlags : VkPipelineViewportStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Rasterization creation flags */
    enum class RasterizationCreateFlags : VkPipelineRasterizationStateCreateFlags
    {
        None = 0x0
    };

    /** @brief Cull mode flags */
    enum class CullModeFlags : VkCullModeFlags
    {
        None = 0x0,
        Front = VK_CULL_MODE_FRONT_BIT,
        Back = VK_CULL_MODE_BACK_BIT,
        FrontAndBack = VK_CULL_MODE_FRONT_AND_BACK
    };


    /** @brief Multisample creation flags */
    enum class MultisampleCreateFlags : VkPipelineMultisampleStateCreateFlags
    {
        None = 0x0
    };

    /** @brief Sample count flags */
    enum class SampleCountFlags : VkSampleCountFlags
    {
        X1 = VK_SAMPLE_COUNT_1_BIT,
        X2 = VK_SAMPLE_COUNT_2_BIT,
        X4 = VK_SAMPLE_COUNT_4_BIT,
        X8 = VK_SAMPLE_COUNT_8_BIT,
        X16 = VK_SAMPLE_COUNT_16_BIT,
        X32 = VK_SAMPLE_COUNT_32_BIT,
        X64 = VK_SAMPLE_COUNT_64_BIT
    };


    /** @brief Depth stencil creation flags */
    enum class DepthStencilCreateFlags : VkPipelineDepthStencilStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Color blend creation flags */
    enum class ColorBlendCreateFlags : VkPipelineColorBlendStateCreateFlags
    {
        None = 0x0
    };

    /** @brief Color components flags */
    enum class ColorComponentFlags : VkColorComponentFlags
    {
        None = 0x0,
        R = VK_COLOR_COMPONENT_R_BIT,
        G = VK_COLOR_COMPONENT_G_BIT,
        B = VK_COLOR_COMPONENT_B_BIT,
        A = VK_COLOR_COMPONENT_A_BIT,
        RGBA = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };


    /** @brief Dynamic state creation flags */
    enum class DynamicStateCreateFlags : VkPipelineDynamicStateCreateFlags
    {
        None = 0x0
    };


    /** @brief Descriptor set layout creation flags */
    enum class DescriptorSetLayoutCreateFlags : VkDescriptorSetLayoutCreateFlags
    {
        None = 0x0,
        UpdateAfterBindPool = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT
    };

    /** @brief Descriptor pool creation flags */
    enum class DescriptorPoolCreateFlags : VkDescriptorPoolCreateFlags
    {
        None = 0x0,
        FreeDescriptorSet = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        UpdateAfterBind = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
    };

    /** @brief Descriptor binding flags */
    enum class DescriptorBindingFlags : VkDescriptorBindingFlags
    {
        UpdateAfterBind = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        UpdateUnusedWhilePending = VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT,
        PartiallyBound = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT,
        VariableDescriptorCount = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
    };


    /** @brief Dependency flags */
    enum class DependencyFlags : VkDependencyFlags
    {
        None = 0x0,
        ByRegion = VK_DEPENDENCY_BY_REGION_BIT,
        DeviceGroup = VK_DEPENDENCY_DEVICE_GROUP_BIT,
        ViewLocal = VK_DEPENDENCY_VIEW_LOCAL_BIT
    };

    /** @brief Access flags */
    enum class AccessFlags : VkAccessFlags
    {
        None = 0x0,
        IndirectCommandRead = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
        IndexRead = VK_ACCESS_INDEX_READ_BIT,
        VertexAttributeRead = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        UniformRead = VK_ACCESS_UNIFORM_READ_BIT,
        InputAttachmentRead = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
        ShaderRead = VK_ACCESS_SHADER_READ_BIT,
        ShaderWrite = VK_ACCESS_SHADER_WRITE_BIT,
        ColorAttachmentRead = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        ColorAttachmentWrite = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        DepthStencilAttachmentRead = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
        DepthStencilAttachmentWrite = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        TransferRead = VK_ACCESS_TRANSFER_READ_BIT,
        TransferWrite = VK_ACCESS_TRANSFER_WRITE_BIT,
        HostRead = VK_ACCESS_HOST_READ_BIT,
        HostWrite = VK_ACCESS_HOST_WRITE_BIT,
        MemoryRead = VK_ACCESS_MEMORY_READ_BIT,
        MemoryWrite = VK_ACCESS_MEMORY_WRITE_BIT
    };

    /** @brief Command pool create flags */
    enum class CommandPoolCreateFlags : VkCommandPoolCreateFlags
    {
        None = 0x0,
        Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        ResetCommandBuffer = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT
    };

    /** @brief Command pool reset flags */
    enum class CommandPoolResetFlags : VkCommandPoolResetFlags
    {
        None = 0x0,
        ReleaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
    };
}

// Enums declarations
namespace kF::GPU
{
    /** @brief Describes all supported queue types */
    enum class QueueType : std::uint32_t {
        Graphics = 0u,
        Compute,
        Transfer,
        Present,
        Count
    };

    /** @brief Practical way to retreive queue count */
    static constexpr auto QueueCount = static_cast<std::size_t>(QueueType::Count);

    /** @brief An index sequence per queue type */
    constexpr std::make_index_sequence<QueueCount> PerQueueIndexSequence {};


    /** @brief Attachment load operator */
    enum class AttachmentLoadOp : std::underlying_type_t<VkAttachmentLoadOp>
    {
        Load = VK_ATTACHMENT_LOAD_OP_LOAD,
        Clear = VK_ATTACHMENT_LOAD_OP_CLEAR,
        DontCare = VK_ATTACHMENT_LOAD_OP_DONT_CARE
    };

    /** @brief Attachment store operator */
    enum class AttachmentStoreOp : std::underlying_type_t<VkAttachmentStoreOp>
    {
        Store = VK_ATTACHMENT_STORE_OP_STORE,
        DontCare = VK_ATTACHMENT_STORE_OP_DONT_CARE
    };


    /** @brief Presentation mode */
    enum class PresentMode : std::underlying_type_t<VkPresentModeKHR>
    {
        ImmediateKhr = VK_PRESENT_MODE_IMMEDIATE_KHR,
        MailboxKhr = VK_PRESENT_MODE_MAILBOX_KHR,
        FifoKhr = VK_PRESENT_MODE_FIFO_KHR,
        FifoRelaxedKhr = VK_PRESENT_MODE_FIFO_RELAXED_KHR
    };


    /** @brief Memory usage */
    enum class MemoryUsage : std::underlying_type_t<VmaMemoryUsage>
    {
        Unknown = VMA_MEMORY_USAGE_UNKNOWN,
        GpuOnly = VMA_MEMORY_USAGE_GPU_ONLY,
        CpuOnly = VMA_MEMORY_USAGE_CPU_ONLY,
        CpuToGpu = VMA_MEMORY_USAGE_CPU_TO_GPU,
        GpuToCpu = VMA_MEMORY_USAGE_GPU_TO_CPU,
        CpuCopy = VMA_MEMORY_USAGE_CPU_COPY,
        GpuLazilyAllocated = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED
    };


    /** @brief Descriptor type */
    enum class DescriptorType : std::underlying_type_t<VkDescriptorType>
    {
        Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
        SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        UniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
        StorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
        UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
        InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
    };


    /** @brief Command buffer level */
    enum class CommandLevel : std::underlying_type_t<VkCommandBufferLevel>
    {
        Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
    };


    /** @brief Sharing mode */
    enum class SharingMode : std::underlying_type_t<VkSharingMode>
    {
        Exclusive = VK_SHARING_MODE_EXCLUSIVE,
        Concurrent = VK_SHARING_MODE_CONCURRENT
    };


    /** @brief Sharing mode */
    enum class ComponentSwizzle : std::underlying_type_t<VkComponentSwizzle>
    {
        Identity = VK_COMPONENT_SWIZZLE_IDENTITY,
        Zero = VK_COMPONENT_SWIZZLE_ZERO,
        One = VK_COMPONENT_SWIZZLE_ONE,
        R = VK_COMPONENT_SWIZZLE_R,
        G = VK_COMPONENT_SWIZZLE_G,
        B = VK_COMPONENT_SWIZZLE_B,
        A = VK_COMPONENT_SWIZZLE_A
    };


    /** @brief Image tiling */
    enum class ImageTiling : std::underlying_type_t<VkImageTiling>
    {
        TilingOptimal = VK_IMAGE_TILING_OPTIMAL,
        TilingLinear = VK_IMAGE_TILING_LINEAR
    };

    /** @brief Image type */
    enum class ImageType : std::underlying_type_t<VkImageType>
    {
        Type1D = VK_IMAGE_TYPE_1D,
        Type2D = VK_IMAGE_TYPE_2D,
        Type3D = VK_IMAGE_TYPE_3D
    };

    /** @brief Image view type */
    enum class ImageViewType : std::underlying_type_t<VkImageViewType>
    {
        Image1D = VK_IMAGE_VIEW_TYPE_1D,
        Image2D = VK_IMAGE_VIEW_TYPE_2D,
        Image3D = VK_IMAGE_VIEW_TYPE_3D,
        Cube = VK_IMAGE_VIEW_TYPE_CUBE,
        Array1D = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
        Array2D = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        ArrayCube = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
    };

    /** @brief Image layout */
    enum class ImageLayout : std::underlying_type_t<VkImageLayout>
    {
        Undefined = VK_IMAGE_LAYOUT_UNDEFINED,
        General = VK_IMAGE_LAYOUT_GENERAL,
        ColorAttachmentOptimal = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        DepthStencilAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        DepthStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        ShaderReadOnlyOptimal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        TransferSrcOptimal = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        TransferDstOptimal = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        Preinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED,
        DepthReadOnlyStencilAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
        DepthAttachmentStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
        DepthAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        DepthReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
        StencilAttachmentOptimal = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        StencilReadOnlyOptimal = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
        ReadOnlyOptimal = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
        AttachmentOptimal = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        PresentSrcKhr = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };


    /** @brief Sampler mimap mode */
    enum class SamplerMipmapMode : std::underlying_type_t<VkSamplerMipmapMode>
    {
        Nearest = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        Linear = VK_SAMPLER_MIPMAP_MODE_LINEAR
    };

    /** @brief Sampler adress mode */
    enum class SamplerAddressMode : std::underlying_type_t<VkSamplerAddressMode>
    {
        Repeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        MirroredRepeat = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
        ClampToEdge = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        ClampToBorder = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        MirrorClampToEdge = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
        MirrorClampToEdgeKhr = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR
    };


    /** @brief Sampler filter */
    enum class Filter : std::underlying_type_t<VkFilter>
    {
        Nearest = VK_FILTER_NEAREST,
        Linear = VK_FILTER_LINEAR,
        CubicImg = VK_FILTER_CUBIC_IMG
    };

    /** @brief Compare operators */
    enum class CompareOp : std::underlying_type_t<VkCompareOp>
    {
        Never = VK_COMPARE_OP_NEVER,
        Less = VK_COMPARE_OP_LESS,
        Equal = VK_COMPARE_OP_EQUAL,
        LessOrEqual = VK_COMPARE_OP_LESS_OR_EQUAL,
        Greater = VK_COMPARE_OP_GREATER,
        NotEqual = VK_COMPARE_OP_NOT_EQUAL,
        GreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
        Always = VK_COMPARE_OP_ALWAYS
    };

    /** @brief Logic operators */
    enum class LogicOp : std::underlying_type_t<VkLogicOp>
    {
        Clear = VK_LOGIC_OP_CLEAR,
        And = VK_LOGIC_OP_AND,
        AndReverse = VK_LOGIC_OP_AND_REVERSE,
        Copy = VK_LOGIC_OP_COPY,
        AndInverted = VK_LOGIC_OP_AND_INVERTED,
        NoOp = VK_LOGIC_OP_NO_OP,
        Xor = VK_LOGIC_OP_XOR,
        Or = VK_LOGIC_OP_OR,
        Nor = VK_LOGIC_OP_NOR,
        Equivalent = VK_LOGIC_OP_EQUIVALENT,
        Invert = VK_LOGIC_OP_INVERT,
        OrReverse = VK_LOGIC_OP_OR_REVERSE,
        CopyInverted = VK_LOGIC_OP_COPY_INVERTED,
        OrInverted = VK_LOGIC_OP_OR_INVERTED,
        Nand = VK_LOGIC_OP_NAND,
        Set = VK_LOGIC_OP_SET,
    };

    /** @brief Stencil operators */
    enum class StencilOp : std::underlying_type_t<VkStencilOp>
    {
        Keep = VK_STENCIL_OP_KEEP,
        Zero = VK_STENCIL_OP_ZERO,
        Replace = VK_STENCIL_OP_REPLACE,
        IncrementAndClamp = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
        DecrementAndClamp = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
        Invert = VK_STENCIL_OP_INVERT,
        IncrementAndWrap = VK_STENCIL_OP_INCREMENT_AND_WRAP,
        DecrementAndWrap = VK_STENCIL_OP_DECREMENT_AND_WRAP
    };

    /** @brief Blend operators */
    enum class BlendOp : std::underlying_type_t<VkBlendOp>
    {
        Add = VK_BLEND_OP_ADD,
        Subtract = VK_BLEND_OP_SUBTRACT,
        ReverseSubtract = VK_BLEND_OP_REVERSE_SUBTRACT,
        Min = VK_BLEND_OP_MIN,
        Max = VK_BLEND_OP_MAX,
        ZeroExt = VK_BLEND_OP_ZERO_EXT,
        SrcExt = VK_BLEND_OP_SRC_EXT,
        DstExt = VK_BLEND_OP_DST_EXT,
        SrcOverExt = VK_BLEND_OP_SRC_OVER_EXT,
        DstOverExt = VK_BLEND_OP_DST_OVER_EXT,
        SrcInExt = VK_BLEND_OP_SRC_IN_EXT,
        DstInExt = VK_BLEND_OP_DST_IN_EXT,
        SrcOutExt = VK_BLEND_OP_SRC_OUT_EXT,
        DstOutExt = VK_BLEND_OP_DST_OUT_EXT,
        SrcAtopExt = VK_BLEND_OP_SRC_ATOP_EXT,
        DstAtopExt = VK_BLEND_OP_DST_ATOP_EXT,
        XorExt = VK_BLEND_OP_XOR_EXT,
        MultiplyExt = VK_BLEND_OP_MULTIPLY_EXT,
        ScreenExt = VK_BLEND_OP_SCREEN_EXT,
        OverlayExt = VK_BLEND_OP_OVERLAY_EXT,
        DarkenExt = VK_BLEND_OP_DARKEN_EXT,
        LightenExt = VK_BLEND_OP_LIGHTEN_EXT,
        ColordodgeExt = VK_BLEND_OP_COLORDODGE_EXT,
        ColorburnExt = VK_BLEND_OP_COLORBURN_EXT,
        HardlightExt = VK_BLEND_OP_HARDLIGHT_EXT,
        SoftlightExt = VK_BLEND_OP_SOFTLIGHT_EXT,
        DifferenceExt = VK_BLEND_OP_DIFFERENCE_EXT,
        ExclusionExt = VK_BLEND_OP_EXCLUSION_EXT,
        InvertExt = VK_BLEND_OP_INVERT_EXT,
        InvertRgbExt = VK_BLEND_OP_INVERT_RGB_EXT,
        LineardodgeExt = VK_BLEND_OP_LINEARDODGE_EXT,
        LinearburnExt = VK_BLEND_OP_LINEARBURN_EXT,
        VividlightExt = VK_BLEND_OP_VIVIDLIGHT_EXT,
        LinearlightExt = VK_BLEND_OP_LINEARLIGHT_EXT,
        PinlightExt = VK_BLEND_OP_PINLIGHT_EXT,
        HardmixExt = VK_BLEND_OP_HARDMIX_EXT,
        HslHueExt = VK_BLEND_OP_HSL_HUE_EXT,
        HslSaturationExt = VK_BLEND_OP_HSL_SATURATION_EXT,
        HslColorExt = VK_BLEND_OP_HSL_COLOR_EXT,
        HslLuminosityExt = VK_BLEND_OP_HSL_LUMINOSITY_EXT,
        PlusExt = VK_BLEND_OP_PLUS_EXT,
        PlusClampedExt = VK_BLEND_OP_PLUS_CLAMPED_EXT,
        PlusClampedAlphaExt = VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT,
        PlusDarkerExt = VK_BLEND_OP_PLUS_DARKER_EXT,
        MinusExt = VK_BLEND_OP_MINUS_EXT,
        MinusClampedExt = VK_BLEND_OP_MINUS_CLAMPED_EXT,
        ContrastExt = VK_BLEND_OP_CONTRAST_EXT,
        InvertOvgExt = VK_BLEND_OP_INVERT_OVG_EXT,
        RedExt = VK_BLEND_OP_RED_EXT,
        GreenExt = VK_BLEND_OP_GREEN_EXT,
        BlueExt = VK_BLEND_OP_BLUE_EXT
    };


    /** @brief Blend factor */
    enum class BlendFactor : std::underlying_type_t<VkBlendFactor>
    {
        Zero = VK_BLEND_FACTOR_ZERO,
        One = VK_BLEND_FACTOR_ONE,
        SrcColor = VK_BLEND_FACTOR_SRC_COLOR,
        OneMinusSrcColor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
        DstColor = VK_BLEND_FACTOR_DST_COLOR,
        OneMinusDstColor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
        SrcAlpha = VK_BLEND_FACTOR_SRC_ALPHA,
        OneMinusSrcAlpha = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        DstAlpha = VK_BLEND_FACTOR_DST_ALPHA,
        OneMinusDstAlpha = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
        ConstantColor = VK_BLEND_FACTOR_CONSTANT_COLOR,
        OneMinusConstantColor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = VK_BLEND_FACTOR_CONSTANT_ALPHA,
        OneMinusConstantAlpha = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
        Src1Color = VK_BLEND_FACTOR_SRC1_COLOR,
        OneMinusSrc1Color = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
        Src1Alpha = VK_BLEND_FACTOR_SRC1_ALPHA,
        OneMinusSrc1Alpha = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
    };


    /** @brief Border color */
    enum class BorderColor : std::underlying_type_t<VkBorderColor>
    {
        FloatTransparentBlack = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        IntTransparentBlack = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        FloatOpaqueBlack = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        IntOpaqueBlack = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        FloatOpaqueWhite = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        IntOpaqueWhite = VK_BORDER_COLOR_INT_OPAQUE_WHITE
    };


    /** @brief Vertex input rate */
    enum class VertexInputRate : std::underlying_type_t<VkVertexInputRate>
    {
        Vertex = VK_VERTEX_INPUT_RATE_VERTEX,
        Instance = VK_VERTEX_INPUT_RATE_INSTANCE
    };

    /** @brief Primitive rendering topologies */
    enum class PrimitiveTopology : std::underlying_type_t<VkPrimitiveTopology>
    {
        PointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        LineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        LineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        TriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        TriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        TriangleFan = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
        LineListWithAdjacency = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
        LineStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
        TriangleListWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
        TriangleStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
        PatchList = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
    };

    /** @brief Polygon modes */
    enum class PolygonMode : std::underlying_type_t<VkPolygonMode>
    {
        Fill = VK_POLYGON_MODE_FILL,
        Line = VK_POLYGON_MODE_LINE,
        Point = VK_POLYGON_MODE_POINT
    };

    /** @brief Front face */
    enum class FrontFace : std::underlying_type_t<VkFrontFace>
    {
        CounterClockwise = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        Clockwise = VK_FRONT_FACE_CLOCKWISE
    };


    /** @brief Dynamic states */
    enum class DynamicState : std::underlying_type_t<VkDynamicState>
    {
        Viewport = VK_DYNAMIC_STATE_VIEWPORT,
        Scissor = VK_DYNAMIC_STATE_SCISSOR,
        LineWidth = VK_DYNAMIC_STATE_LINE_WIDTH,
        DepthBias = VK_DYNAMIC_STATE_DEPTH_BIAS,
        BlendConstants = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        DepthBounds = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        StencilCompareMask = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
        StencilWriteMask = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        StencilReference = VK_DYNAMIC_STATE_STENCIL_REFERENCE
    };

    /** @brief Subpass contents */
    enum class SubpassContents : std::underlying_type_t<VkSubpassContents>
    {
        Inline = VK_SUBPASS_CONTENTS_INLINE,
        SecondaryCommandBuffers = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
    };

    /** @brief Index buffer type */
    enum class IndexType : std::underlying_type_t<VkIndexType>
    {
        Uint16 = VK_INDEX_TYPE_UINT16,
        Uint32 = VK_INDEX_TYPE_UINT32
    };

    /** @brief Pipeline bind point */
    enum class PipelineBindPoint : std::underlying_type_t<VkPipelineBindPoint>
    {
        Graphics = VK_PIPELINE_BIND_POINT_GRAPHICS,
        Compute = VK_PIPELINE_BIND_POINT_COMPUTE
    };


    /** @brief All supported bytes formats */
    enum class Format : std::underlying_type_t<VkFormat>
    {
        Undefined = VK_FORMAT_UNDEFINED,
        R4G4_UNORM_PACK8 = VK_FORMAT_R4G4_UNORM_PACK8,
        R4G4B4A4_UNORM_PACK16 = VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        B4G4R4A4_UNORM_PACK16 = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        R5G6B5_UNORM_PACK16 = VK_FORMAT_R5G6B5_UNORM_PACK16,
        B5G6R5_UNORM_PACK16 = VK_FORMAT_B5G6R5_UNORM_PACK16,
        R5G5B5A1_UNORM_PACK16 = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        B5G5R5A1_UNORM_PACK16 = VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        A1R5G5B5_UNORM_PACK16 = VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        R8_UNORM = VK_FORMAT_R8_UNORM,
        R8_SNORM = VK_FORMAT_R8_SNORM,
        R8_USCALED = VK_FORMAT_R8_USCALED,
        R8_SSCALED = VK_FORMAT_R8_SSCALED,
        R8_UINT = VK_FORMAT_R8_UINT,
        R8_SINT = VK_FORMAT_R8_SINT,
        R8_SRGB = VK_FORMAT_R8_SRGB,
        R8G8_UNORM = VK_FORMAT_R8G8_UNORM,
        R8G8_SNORM = VK_FORMAT_R8G8_SNORM,
        R8G8_USCALED = VK_FORMAT_R8G8_USCALED,
        R8G8_SSCALED = VK_FORMAT_R8G8_SSCALED,
        R8G8_UINT = VK_FORMAT_R8G8_UINT,
        R8G8_SINT = VK_FORMAT_R8G8_SINT,
        R8G8_SRGB = VK_FORMAT_R8G8_SRGB,
        R8G8B8_UNORM = VK_FORMAT_R8G8B8_UNORM,
        R8G8B8_SNORM = VK_FORMAT_R8G8B8_SNORM,
        R8G8B8_USCALED = VK_FORMAT_R8G8B8_USCALED,
        R8G8B8_SSCALED = VK_FORMAT_R8G8B8_SSCALED,
        R8G8B8_UINT = VK_FORMAT_R8G8B8_UINT,
        R8G8B8_SINT = VK_FORMAT_R8G8B8_SINT,
        R8G8B8_SRGB = VK_FORMAT_R8G8B8_SRGB,
        B8G8R8_UNORM = VK_FORMAT_B8G8R8_UNORM,
        B8G8R8_SNORM = VK_FORMAT_B8G8R8_SNORM,
        B8G8R8_USCALED = VK_FORMAT_B8G8R8_USCALED,
        B8G8R8_SSCALED = VK_FORMAT_B8G8R8_SSCALED,
        B8G8R8_UINT = VK_FORMAT_B8G8R8_UINT,
        B8G8R8_SINT = VK_FORMAT_B8G8R8_SINT,
        B8G8R8_SRGB = VK_FORMAT_B8G8R8_SRGB,
        R8G8B8A8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
        R8G8B8A8_SNORM = VK_FORMAT_R8G8B8A8_SNORM,
        R8G8B8A8_USCALED = VK_FORMAT_R8G8B8A8_USCALED,
        R8G8B8A8_SSCALED = VK_FORMAT_R8G8B8A8_SSCALED,
        R8G8B8A8_UINT = VK_FORMAT_R8G8B8A8_UINT,
        R8G8B8A8_SINT = VK_FORMAT_R8G8B8A8_SINT,
        R8G8B8A8_SRGB = VK_FORMAT_R8G8B8A8_SRGB,
        B8G8R8A8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
        B8G8R8A8_SNORM = VK_FORMAT_B8G8R8A8_SNORM,
        B8G8R8A8_USCALED = VK_FORMAT_B8G8R8A8_USCALED,
        B8G8R8A8_SSCALED = VK_FORMAT_B8G8R8A8_SSCALED,
        B8G8R8A8_UINT = VK_FORMAT_B8G8R8A8_UINT,
        B8G8R8A8_SINT = VK_FORMAT_B8G8R8A8_SINT,
        B8G8R8A8_SRGB = VK_FORMAT_B8G8R8A8_SRGB,
        A8B8G8R8_UNORM_PACK32 = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        A8B8G8R8_SNORM_PACK32 = VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        A8B8G8R8_USCALED_PACK32 = VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        A8B8G8R8_SSCALED_PACK32 = VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        A8B8G8R8_UINT_PACK32 = VK_FORMAT_A8B8G8R8_UINT_PACK32,
        A8B8G8R8_SINT_PACK32 = VK_FORMAT_A8B8G8R8_SINT_PACK32,
        A8B8G8R8_SRGB_PACK32 = VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        A2R10G10B10_UNORM_PACK32 = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        A2R10G10B10_SNORM_PACK32 = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        A2R10G10B10_USCALED_PACK32 = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        A2R10G10B10_SSCALED_PACK32 = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        A2R10G10B10_UINT_PACK32 = VK_FORMAT_A2R10G10B10_UINT_PACK32,
        A2R10G10B10_SINT_PACK32 = VK_FORMAT_A2R10G10B10_SINT_PACK32,
        A2B10G10R10_UNORM_PACK32 = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        A2B10G10R10_SNORM_PACK32 = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        A2B10G10R10_USCALED_PACK32 = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        A2B10G10R10_SSCALED_PACK32 = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        A2B10G10R10_UINT_PACK32 = VK_FORMAT_A2B10G10R10_UINT_PACK32,
        A2B10G10R10_SINT_PACK32 = VK_FORMAT_A2B10G10R10_SINT_PACK32,
        R16_UNORM = VK_FORMAT_R16_UNORM,
        R16_SNORM = VK_FORMAT_R16_SNORM,
        R16_USCALED = VK_FORMAT_R16_USCALED,
        R16_SSCALED = VK_FORMAT_R16_SSCALED,
        R16_UINT = VK_FORMAT_R16_UINT,
        R16_SINT = VK_FORMAT_R16_SINT,
        R16_SFLOAT = VK_FORMAT_R16_SFLOAT,
        R16G16_UNORM = VK_FORMAT_R16G16_UNORM,
        R16G16_SNORM = VK_FORMAT_R16G16_SNORM,
        R16G16_USCALED = VK_FORMAT_R16G16_USCALED,
        R16G16_SSCALED = VK_FORMAT_R16G16_SSCALED,
        R16G16_UINT = VK_FORMAT_R16G16_UINT,
        R16G16_SINT = VK_FORMAT_R16G16_SINT,
        R16G16_SFLOAT = VK_FORMAT_R16G16_SFLOAT,
        R16G16B16_UNORM = VK_FORMAT_R16G16B16_UNORM,
        R16G16B16_SNORM = VK_FORMAT_R16G16B16_SNORM,
        R16G16B16_USCALED = VK_FORMAT_R16G16B16_USCALED,
        R16G16B16_SSCALED = VK_FORMAT_R16G16B16_SSCALED,
        R16G16B16_UINT = VK_FORMAT_R16G16B16_UINT,
        R16G16B16_SINT = VK_FORMAT_R16G16B16_SINT,
        R16G16B16_SFLOAT = VK_FORMAT_R16G16B16_SFLOAT,
        R16G16B16A16_UNORM = VK_FORMAT_R16G16B16A16_UNORM,
        R16G16B16A16_SNORM = VK_FORMAT_R16G16B16A16_SNORM,
        R16G16B16A16_USCALED = VK_FORMAT_R16G16B16A16_USCALED,
        R16G16B16A16_SSCALED = VK_FORMAT_R16G16B16A16_SSCALED,
        R16G16B16A16_UINT = VK_FORMAT_R16G16B16A16_UINT,
        R16G16B16A16_SINT = VK_FORMAT_R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT = VK_FORMAT_R16G16B16A16_SFLOAT,
        R32_UINT = VK_FORMAT_R32_UINT,
        R32_SINT = VK_FORMAT_R32_SINT,
        R32_SFLOAT = VK_FORMAT_R32_SFLOAT,
        R32G32_UINT = VK_FORMAT_R32G32_UINT,
        R32G32_SINT = VK_FORMAT_R32G32_SINT,
        R32G32_SFLOAT = VK_FORMAT_R32G32_SFLOAT,
        R32G32B32_UINT = VK_FORMAT_R32G32B32_UINT,
        R32G32B32_SINT = VK_FORMAT_R32G32B32_SINT,
        R32G32B32_SFLOAT = VK_FORMAT_R32G32B32_SFLOAT,
        R32G32B32A32_UINT = VK_FORMAT_R32G32B32A32_UINT,
        R32G32B32A32_SINT = VK_FORMAT_R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT = VK_FORMAT_R32G32B32A32_SFLOAT,
        R64_UINT = VK_FORMAT_R64_UINT,
        R64_SINT = VK_FORMAT_R64_SINT,
        R64_SFLOAT = VK_FORMAT_R64_SFLOAT,
        R64G64_UINT = VK_FORMAT_R64G64_UINT,
        R64G64_SINT = VK_FORMAT_R64G64_SINT,
        R64G64_SFLOAT = VK_FORMAT_R64G64_SFLOAT,
        R64G64B64_UINT = VK_FORMAT_R64G64B64_UINT,
        R64G64B64_SINT = VK_FORMAT_R64G64B64_SINT,
        R64G64B64_SFLOAT = VK_FORMAT_R64G64B64_SFLOAT,
        R64G64B64A64_UINT = VK_FORMAT_R64G64B64A64_UINT,
        R64G64B64A64_SINT = VK_FORMAT_R64G64B64A64_SINT,
        R64G64B64A64_SFLOAT = VK_FORMAT_R64G64B64A64_SFLOAT,
        B10G11R11_UFLOAT_PACK32 = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        E5B9G9R9_UFLOAT_PACK32 = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        D16_UNORM = VK_FORMAT_D16_UNORM,
        X8_D24_UNORM_PACK32 = VK_FORMAT_X8_D24_UNORM_PACK32,
        D32_SFLOAT = VK_FORMAT_D32_SFLOAT,
        S8_UINT = VK_FORMAT_S8_UINT,
        D16_UNORM_S8_UINT = VK_FORMAT_D16_UNORM_S8_UINT,
        D24_UNORM_S8_UINT = VK_FORMAT_D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT = VK_FORMAT_D32_SFLOAT_S8_UINT
    };
}

// Function declarations
namespace kF::GPU
{
    /** @brief Get an error message from a vulkan result */
    [[nodiscard]] std::string_view ErrorMessage(const VkResult res) noexcept;

    /** @brief Get a literal of QueueType enumeration */
    [[nodiscard]] std::string_view QueueTypeName(const QueueType type) noexcept;

    /** @brief Get literal of a 'PresentMode' enumeration */
    [[nodiscard]] std::string_view PresentModeName(const PresentMode type) noexcept;

    /** @brief Get literal of a 'Format' enumeration */
    [[nodiscard]] std::string_view FormatName(const Format type) noexcept;


    namespace Internal
    {
        /** @brief Fill a container using a vulkan style getter with VkResult as return type */
        template<typename Container, typename ...FunctionArgs, typename ...Args>
        [[nodiscard]] VkResult FillVkContainer(Container &container, VkResult(*function)(FunctionArgs...), Args &&...args) noexcept;

        /** @brief Fill a container using a vulkan style getter with void as return type */
        template<typename Container, typename ...FunctionArgs, typename ...Args>
        void FillVkContainer(Container &container, void(*function)(FunctionArgs...), Args &&...args) noexcept;
    }
}

#include "Base.ipp"