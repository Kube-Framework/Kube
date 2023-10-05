/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FrameImageModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct FrameImageModel;

    /** @brief A list of frame images that describe a Framebuffer */
    using FrameImageModels = Core::Vector<FrameImageModel, GPUAllocator>;
};

/** @brief Describe a frame image */
struct alignas_cacheline kF::GPU::FrameImageModel
{
    ImageTiling imageTiling;
    FormatFeatureFlags formatFeatureFlags;
    ImageAspectFlags imageAspectFlags;
    ImageUsageFlags imageUsageFlags;
    ComponentMapping componentMapping;
    Core::SmallVector<Format, Core::CacheLineQuarterSize / sizeof(Format), GPUAllocator> supportedFormats;


    /** @brief Destructor */
    ~FrameImageModel(void) noexcept = default;

    /** @brief Initialize constructor */
    FrameImageModel(const ImageTiling imageTiling_,
            const FormatFeatureFlags formatFeatureFlags_,
            const ImageAspectFlags imageAspectFlags_,
            const ImageUsageFlags imageUsageFlags_,
            const ComponentMapping &componentMapping_,
            const std::initializer_list<Format> &supportedFormats_) noexcept
        :   imageTiling(imageTiling_),
            formatFeatureFlags(formatFeatureFlags_),
            imageAspectFlags(imageAspectFlags_),
            imageUsageFlags(imageUsageFlags_),
            componentMapping(componentMapping_),
            supportedFormats(supportedFormats_.begin(), supportedFormats_.end()) {}

    /** @brief POD semantics */
    FrameImageModel(const FrameImageModel &other) noexcept = default;
    FrameImageModel(FrameImageModel &&other) noexcept = default;
    FrameImageModel &operator=(const FrameImageModel &other) noexcept = default;
    FrameImageModel &operator=(FrameImageModel &&other) noexcept = default;
};
static_assert_fit_cacheline(kF::GPU::FrameImageModel);
