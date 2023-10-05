/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FramebufferManager
 */

#pragma once

#include "PerFrameCache.hpp"
#include "Framebuffer.hpp"

#include <Kube/Core/SmallVector.hpp>

namespace kF::GPU
{
    class FramebufferManager;
}

/** @brief A framebuffer handler holds a set of framebuffers */
class alignas_half_cacheline kF::GPU::FramebufferManager : public CachedGPUObject
{
public:
    /** @brief Cache of a frame */
    struct alignas_half_cacheline FrameCache
    {
        /** @brief List of Framebuffer */
        using Framebuffers = Core::SmallVector<Framebuffer, Core::CacheLineQuarterSize / sizeof(Framebuffer), GPUAllocator>;

        Framebuffers framebuffers {};
    };
    static_assert_fit_half_cacheline(FrameCache);


    /** @brief Destoy the handler */
    ~FramebufferManager(void) noexcept = default;

    /** @brief Construct a framebuffer handler */
    FramebufferManager(void) noexcept;

    /** @brief Move constructor */
    FramebufferManager(FramebufferManager &&other) noexcept = default;

    /** @brief Move assignment */
    FramebufferManager &operator=(FramebufferManager &&other) noexcept = default;


    /** @brief Retreive the current frame buffer */
    [[nodiscard]] const FrameCache &currentFrameCache(void) const noexcept
        { return _perFrameCache.current(); }

    /** @brief Retreive the current frame buffer */
    [[nodiscard]] const FrameCache &frameCacheAt(const FrameIndex frameIndex) const noexcept
        { return _perFrameCache.at(frameIndex); }

    /** @brief Retreive the current frame buffer at given render pass index */
    [[nodiscard]] FramebufferHandle currentFramebuffer(const std::uint32_t renderPassIndex) const noexcept
        { return _perFrameCache.current().framebuffers.at(renderPassIndex); }

    /** @brief Retreive the current frame buffer at given render pass index */
    [[nodiscard]] FramebufferHandle framebufferAt(const std::uint32_t frameIndex, const std::uint32_t renderPassIndex) const noexcept
        { return _perFrameCache.at(frameIndex).framebuffers.at(renderPassIndex); }


    /** @brief Callback triggered when renderer is processing a new frame */
    inline void onFrameAquired(const FrameIndex frameIndex) noexcept { _perFrameCache.setCurrentFrame(frameIndex); }

    /** @brief Callback on render view size changed */
    inline void onViewSizeChanged(void) noexcept { createFramebuffers(); }

private:
    PerFrameCache<FrameCache, GPUAllocator> _perFrameCache {};

    /** @brief Create framebuffers */
    void createFramebuffers(void) noexcept;
};
static_assert_fit_half_cacheline(kF::GPU::FramebufferManager);
