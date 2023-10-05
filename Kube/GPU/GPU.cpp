/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPU sources
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"

using namespace kF;

GPU::GPU::GlobalInstance::GlobalInstance(
    BackendWindow * const window,
    FrameImageModels &&frameImageModels,
    const std::initializer_list<RenderPassFactory> &renderPassFactories,
    const Core::Version applicationVersion
) noexcept
{
    kFEnsure(!_Constructed,
        "GPU::GPU::GlobalInstance: GPU already instantiated");
    _Constructed = true;
    _instance = new (&GPUObject::Parent()) GPU(window, std::move(frameImageModels), std::move(renderPassFactories), applicationVersion);
}

GPU::GPU::GlobalInstance::~GlobalInstance(void) noexcept
{
    _Constructed = false;
    _instance->~GPU();
}

GPU::GPU::GPU(
    BackendWindow * const window,
    FrameImageModels &&frameImageModels,
    const std::initializer_list<RenderPassFactory> &renderPassFactories,
    const Core::Version applicationVersion
) noexcept
    : _window(window)
    , _instance(applicationVersion)
    , _frameImageManager(std::move(frameImageModels))
    , _renderPassManager(std::move(renderPassFactories))
{
}

void GPU::GPU::dispatchFrameAcquired(const FrameIndex frameIndex) noexcept
{
    _frameImageManager.onFrameAquired(frameIndex);
    _framebufferManager.onFrameAquired(frameIndex);
    _frameAcquiredDispatcher.dispatch(frameIndex);
}

void GPU::GPU::dispatchViewSizeChanged(const bool gpuEvent) noexcept
{
    kFInfo("[GPU] DispatchViewSizeChanged gpuEvent(", gpuEvent, ") swapchain(", swapchain().operator bool(), ")");
    // Only user window event can recover from a minimized to visible scenario
    // If the swapchain exists, we can ignore this event
    if (!gpuEvent && swapchain())
        return;
    _swapchain.onViewSizeChanged();
    _frameImageManager.onViewSizeChanged();
    _renderPassManager.onViewSizeChanged();
    _framebufferManager.onViewSizeChanged();
    _viewSizeDispatcher.dispatch();
}
