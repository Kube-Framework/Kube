/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct ImageModel;
};

/** @brief Describe how to create an image */
struct kF::GPU::ImageModel : public VkImageCreateInfo
{
    /** @brief Helper used to create a single layer 2D image model */
    [[nodiscard]] static ImageModel MakeSingleLayer2D(const Extent2D &extent, const Format format, const ImageUsageFlags customFlags, const ImageTiling tiling) noexcept;

    /** @brief Helper used to create a depth image model */
    [[nodiscard]] static ImageModel MakeDepthStencil(const Extent2D &extent, const Format format, const ImageUsageFlags customFlags = ImageUsageFlags::None) noexcept;


    /** @brief Destructor */
    ~ImageModel(void) noexcept = default;

    /** @brief Initialize constructor */
    ImageModel(
        const ImageCreateFlags flags_,
        const ImageType type_,
        const Format format_,
        const Extent3D &extent_,
        const std::uint32_t mipLevels_,
        const std::uint32_t arrayLayers_,
        const SampleCountFlags samples_,
        const ImageTiling tiling_,
        const ImageUsageFlags usage_,
        const SharingMode sharingMode_,
        const std::uint32_t * const queueFamilyIndexBegin,
        const std::uint32_t * const queueFamilyIndexEnd,
        const ImageLayout initialLayout_
    ) noexcept
        : VkImageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .imageType = static_cast<VkImageType>(type_),
            .format = static_cast<VkFormat>(format_),
            .extent = extent_,
            .mipLevels = mipLevels_,
            .arrayLayers = arrayLayers_,
            .samples = static_cast<VkSampleCountFlagBits>(samples_),
            .tiling = static_cast<VkImageTiling>(tiling_),
            .usage = ToFlags(usage_),
            .sharingMode = static_cast<VkSharingMode>(sharingMode_),
            .queueFamilyIndexCount = Core::Distance<std::uint32_t>(queueFamilyIndexBegin, queueFamilyIndexEnd),
            .pQueueFamilyIndices = queueFamilyIndexBegin,
            .initialLayout = static_cast<VkImageLayout>(initialLayout_)
         } {}

    /** @brief POD semantics */
    ImageModel(const ImageModel &other) noexcept = default;
    ImageModel(ImageModel &&other) noexcept = default;
    ImageModel &operator=(const ImageModel &other) noexcept = default;
    ImageModel &operator=(ImageModel &&other) noexcept = default;
};

#include "ImageModel.ipp"