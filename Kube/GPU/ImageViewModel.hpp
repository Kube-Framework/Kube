/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageViewModel.hpp
 */

#pragma once

#include "Base.hpp"
#include "ComponentMapping.hpp"
#include "ImageSubresourceRange.hpp"

namespace kF::GPU
{
    struct ImageViewModel;
};

/** @brief Describe how to create an image view */
struct kF::GPU::ImageViewModel : public VkImageViewCreateInfo
{
    /** @brief Destructor */
    ~ImageViewModel(void) noexcept = default;

    /** @brief Initialize constructor */
    ImageViewModel(const ImageViewCreateFlags flags_, const ImageHandle image_, const ImageViewType viewType_, const Format format_,
            const ComponentMapping &components_, const ImageSubresourceRange &subresource_) noexcept
        : VkImageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .image = image_,
            .viewType = static_cast<VkImageViewType>(viewType_),
            .format = static_cast<VkFormat>(format_),
            .components = components_,
            .subresourceRange = subresource_
        } {}

    /** @brief POD semantics */
    ImageViewModel(const ImageViewModel &other) noexcept = default;
    ImageViewModel(ImageViewModel &&other) noexcept = default;
    ImageViewModel &operator=(const ImageViewModel &other) noexcept = default;
    ImageViewModel &operator=(ImageViewModel &&other) noexcept = default;
};
