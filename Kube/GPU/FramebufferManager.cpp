/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Framebuffer
 */

#include "FramebufferManager.hpp"
#include "GPU.hpp"

using namespace kF;

GPU::FramebufferManager::FramebufferManager(void) noexcept
{
    createFramebuffers();
}

void GPU::FramebufferManager::createFramebuffers(void) noexcept
{
    // Return if there is no swapchain
    if (!parent().swapchain()) {
        _perFrameCache.release();
        return;
    }

    // Query swapchain images & data
    const auto extent = parent().swapchain().extent();
    const auto &frameImageManager = parent().frameImageManager();

    // Create image view list
    Core::Vector<ImageViewHandle, GPUAllocator> imageViewHandles(frameImageManager.perFrameImageCount());

    // Create a FrameCache for each frame
    _perFrameCache.resize(frameImageManager.frameCount(),
        [this, &extent, &frameImageManager, &imageViewHandles](const auto index) {
            FrameCache cache;
            auto &frameImageCache = frameImageManager.frameCacheAt(index);

            // Update image view handles
            auto it = imageViewHandles.begin();
            const auto end = imageViewHandles.end();
            *it = frameImageCache.colorView;
            ++it;
            for (auto customImageIndex = 0u; it != end; ++it, ++customImageIndex)
                *it = frameImageCache.customImages.at(customImageIndex).view;

            // Create a Framebuffer for each RenderPass
            cache.framebuffers.resize(parent().renderPassManager().renderPassCount(),
                [this, &extent, &imageViewHandles](const auto index) {
                    // Setup framebuffer
                    return FramebufferModel(
                        FramebufferCreateFlags::None,
                        parent().renderPassManager().renderPassAt(index),
                        imageViewHandles.begin(), imageViewHandles.end(),
                        extent.width, extent.height, 1u
                    );
                }
            );
            return cache;
        }
    );
}