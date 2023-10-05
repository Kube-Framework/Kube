/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FrameImageManager
 */

#pragma once

#include "PerFrameCache.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "MemoryAllocation.hpp"
#include "FrameImageModel.hpp"

namespace kF::GPU
{
    class FrameImageManager;
}

/** @brief A FrameImageManager holds a set of images used in Framebuffers */
class alignas_cacheline kF::GPU::FrameImageManager : public CachedGPUObject
{
public:
    /** @brief List of framebuffer image formats */
    using CustomFormats = Core::Vector<Format, GPUAllocator>;

    /** @brief Cache of a custom image within a frame */
    struct alignas_half_cacheline ImageCache
    {
        Image image {};
        ImageView view {};
        MemoryAllocation memoryAllocation {};
        Format format {};
    };
    static_assert_fit_half_cacheline(ImageCache);

    /** @brief Cache of a frame */
    struct alignas_half_cacheline FrameCache
    {
        ImageHandle colorImage {};
        ImageView colorView {};
        Core::Vector<ImageCache, GPUAllocator> customImages {};
    };
    static_assert_fit_half_cacheline(FrameCache);


    /** @brief Destoy the handler */
    ~FrameImageManager(void) noexcept = default;

    /** @brief Construct a framebuffer handler */
    FrameImageManager(FrameImageModels &&models) noexcept;

    /** @brief Move constructor */
    FrameImageManager(FrameImageManager &&other) noexcept = default;

    /** @brief Move assignment */
    FrameImageManager &operator=(FrameImageManager &&other) noexcept = default;


    /** @brief Get the number of cached frames */
    [[nodiscard]] inline FrameIndex frameCount(void) const noexcept { return _perFrameCache.count(); }

    /** @brief Get the number of cached frames */
    [[nodiscard]] inline std::uint32_t perFrameImageCount(void) const noexcept { return _customFormats.size() + 1u; }

    /** @brief Retreive the current frame buffer */
    [[nodiscard]] inline const FrameCache &currentFrameCache(void) const noexcept
        { return _perFrameCache.current(); }

    /** @brief Retreive the current frame buffer */
    [[nodiscard]] inline const FrameCache &frameCacheAt(const FrameIndex index) const noexcept
        { return _perFrameCache.at(index); }

    /** @brief Query custom image formats */
    [[nodiscard]] inline const CustomFormats &customFormats(void) const noexcept { return _customFormats; }


    /** @brief Callback triggered when renderer is processing a new frame */
    inline void onFrameAquired(const FrameIndex frameIndex) noexcept { _perFrameCache.setCurrentFrame(frameIndex); }

    /** @brief Callback on render view size changed */
    inline void onViewSizeChanged(void) noexcept { createFrameImages(); }

private:
    PerFrameCache<FrameCache, GPUAllocator> _perFrameCache {};
    FrameImageModels _models {};
    CustomFormats _customFormats {};

    /** @brief Create frame images */
    void createFrameImages(void) noexcept;
};
static_assert_fit_cacheline(kF::GPU::FrameImageManager);
