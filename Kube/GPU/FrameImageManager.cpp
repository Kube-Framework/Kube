/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Framebuffer
 */

#include "GPU.hpp"
#include "FrameImageManager.hpp"

using namespace kF;

GPU::FrameImageManager::FrameImageManager(FrameImageModels &&models) noexcept
    : _models(std::move(models))
{
    createFrameImages();

#if KUBE_DEBUG_BUILD
    kFInfo("[GPU] Frame count: ", frameCount());
    kFInfo("[GPU] Per frame image count: ", perFrameImageCount());
#endif
}

void GPU::FrameImageManager::createFrameImages(void) noexcept
{
    // Return if there is no swapchain
    if (!parent().swapchain()) {
        _perFrameCache.release();
        _customFormats.clear();
        return;
    }

    // Query swapchain images & data
    const auto extent = parent().swapchain().extent();
    const auto swapchainImages = parent().swapchain().queryImages();

    // Retreive custom images formats
    _customFormats.resize(_models.size(), [this](const std::uint32_t index) {
        const auto &frameImageModel = _models[index];
        return parent().physicalDevice().findSupportedFormat(
            frameImageModel.imageTiling,
            frameImageModel.formatFeatureFlags,
            frameImageModel.supportedFormats.begin(),
            frameImageModel.supportedFormats.end()
        );
    });

    // Create a FrameCache for each frame
    _perFrameCache.resize(swapchainImages.size(),
        [this, &extent, &swapchainImages](const FrameIndex index) {
            FrameCache cache;

            // Create view of surface image
            cache.colorImage = swapchainImages[index];
            cache.colorView = ImageViewModel(
                ImageViewCreateFlags::None,
                cache.colorImage,
                ImageViewType::Image2D,
                static_cast<Format>(parent().swapchain().surfaceFormat().format),
                ComponentMapping(),
                ImageSubresourceRange(ImageAspectFlags::Color)
            );

            // Create images & views of custom model images
            cache.customImages.resize(_models.size(), [this, &extent](const auto index) {
                const auto &frameImageModel = _models[index];
                ImageCache imageCache;

                // Create image
                imageCache.format = _customFormats[index];
                imageCache.image = Image::MakeSingleLayer2D(
                    extent,
                    imageCache.format,
                    frameImageModel.imageUsageFlags,
                    frameImageModel.imageTiling
                );

                // Allocate image
                imageCache.memoryAllocation = MemoryAllocation::MakeLocal(imageCache.image);

                // Create image view
                imageCache.view = ImageViewModel(
                    ImageViewCreateFlags::None,
                    imageCache.image,
                    ImageViewType::Image2D,
                    imageCache.format,
                    frameImageModel.componentMapping,
                    ImageSubresourceRange(frameImageModel.imageAspectFlags)
                );


                return imageCache;
            });

            return cache;
        }
    );
}