/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPU interface
 */

#pragma once

#include <Kube/Core/TrivialDispatcher.hpp>

#include "Instance.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"
#include "QueueManager.hpp"
#include "LogicalDevice.hpp"
#include "Swapchain.hpp"
#include "FrameImageManager.hpp"
#include "RenderPassManager.hpp"
#include "FramebufferManager.hpp"
#include "CommandDispatcher.hpp"
#include "MemoryAllocator.hpp"

namespace kF::GPU
{
    class GPU;
}

/** @brief Instance of the GPU tree, used to access the low-level GPU api */
class alignas_double_cacheline kF::GPU::GPU
{
public:
    /** @brief A GPU cannot be directly created, you must instead use GlobalInstance to handle its lifetime globally */
    class GlobalInstance
    {
    public:
        /** @brief Destroy the global GPU */
        ~GlobalInstance(void) noexcept;

        /** @brief GlobalInstance is not copiable */
        GlobalInstance(const GlobalInstance &other) noexcept = delete;
        GlobalInstance &operator=(const GlobalInstance &other) noexcept = delete;

        /** @brief Construct a global GPU using a backend window */
        GlobalInstance(
            BackendWindow * const window,
            FrameImageModels &&frameImageModels,
            const std::initializer_list<RenderPassFactory> &renderPassFactories,
            const Core::Version applicationVersion = Core::Version(0, 1, 0)
        ) noexcept;


        /** @brief Access operator */
        [[nodiscard]] GPU *operator->(void) noexcept { return _instance; }
        [[nodiscard]] const GPU *operator->(void) const noexcept { return _instance; }

        /** @brief Dereference operator */
        [[nodiscard]] GPU &operator*(void) noexcept { return *_instance; }
        [[nodiscard]] const GPU &operator*(void) const noexcept { return *_instance; }

    private:
        GPU *_instance {};
    };

    /** @brief Destruct the whole GPU tree */
    ~GPU(void) noexcept = default;

    /** @brief GPU is not copiable */
    GPU(const GPU &other) noexcept = delete;
    GPU &operator=(const GPU &other) noexcept = delete;


    /** @brief Get the assigned backend window */
    [[nodiscard]] BackendWindow *backendWindow(void) const noexcept { return _window; }

    /** @brief Get the internal API instance */
    [[nodiscard]] Instance &instance(void) noexcept { return _instance; }
    [[nodiscard]] const Instance &instance(void) const noexcept { return _instance; }

    /** @brief Get the render surface */
    [[nodiscard]] Surface &surface(void) noexcept { return _surface; }
    [[nodiscard]] const Surface &surface(void) const noexcept { return _surface; }

    /** @brief Get the physical device */
    [[nodiscard]] PhysicalDevice &physicalDevice(void) noexcept { return _physicalDevice; }
    [[nodiscard]] const PhysicalDevice &physicalDevice(void) const noexcept { return _physicalDevice; }

    /** @brief Get the queue handler */
    [[nodiscard]] QueueManager &queueManager(void) noexcept { return _queueManager; }
    [[nodiscard]] const QueueManager &queueManager(void) const noexcept { return _queueManager; }

    /** @brief Get the logical device */
    [[nodiscard]] LogicalDevice &logicalDevice(void) noexcept { return _logicalDevice; }
    [[nodiscard]] const LogicalDevice &logicalDevice(void) const noexcept { return _logicalDevice; }

    /** @brief Get the memory allocator */
    [[nodiscard]] MemoryAllocator &memoryAllocator(void) noexcept { return _memoryAllocator; }
    [[nodiscard]] const MemoryAllocator &memoryAllocator(void) const noexcept { return _memoryAllocator; }

    /** @brief Get the swapchain */
    [[nodiscard]] Swapchain &swapchain(void) noexcept { return _swapchain; }
    [[nodiscard]] const Swapchain &swapchain(void) const noexcept { return _swapchain; }

    /** @brief Get the frameImage handler */
    [[nodiscard]] FrameImageManager &frameImageManager(void) noexcept { return _frameImageManager; }
    [[nodiscard]] const FrameImageManager &frameImageManager(void) const noexcept { return _frameImageManager; }

    /** @brief Get the render pass */
    [[nodiscard]] RenderPassManager &renderPassManager(void) noexcept { return _renderPassManager; }
    [[nodiscard]] const RenderPassManager &renderPassManager(void) const noexcept { return _renderPassManager; }

    /** @brief Get the framebuffer handler */
    [[nodiscard]] FramebufferManager &framebufferManager(void) noexcept { return _framebufferManager; }
    [[nodiscard]] const FramebufferManager &framebufferManager(void) const noexcept { return _framebufferManager; }

    /** @brief Get the command dispatcher */
    [[nodiscard]] CommandDispatcher &commandDispatcher(void) noexcept { return _commandDispatcher; }
    [[nodiscard]] const CommandDispatcher &commandDispatcher(void) const noexcept { return _commandDispatcher; }


    /** @brief Get the number of cached frame */
    [[nodiscard]] FrameIndex frameCount(void) const noexcept { return frameImageManager().frameCount(); }


    /** @brief Dispatch every known and registered 'onFrameAcquired' callback */
    void dispatchFrameAcquired(const FrameIndex frameIndex) noexcept;

    /** @brief Dispatch every known and registered 'onViewSizeChanged' callback */
    void dispatchViewSizeChanged(const bool gpuEvent = false) noexcept;


    /** @brief Get the 'onFrameAcquired' dispatcher */
    [[nodiscard]] auto &frameAcquiredDispatcher(void) noexcept { return _frameAcquiredDispatcher; }

    /** @brief Get the 'onViewSizeChanged' dispatcher */
    [[nodiscard]] auto &viewSizeDispatcher(void) noexcept { return _viewSizeDispatcher; }


private:
    BackendWindow *_window {};
    Instance _instance;
    Surface _surface {};
    PhysicalDevice _physicalDevice {};
    QueueManager _queueManager {};
    LogicalDevice _logicalDevice {};
    MemoryAllocator _memoryAllocator {};
    Swapchain _swapchain {};
    FrameImageManager _frameImageManager;
    RenderPassManager _renderPassManager;
    FramebufferManager _framebufferManager {};
    CommandDispatcher _commandDispatcher {};
    Core::TrivialDispatcher<void(const FrameIndex)> _frameAcquiredDispatcher {};
    Core::TrivialDispatcher<void(void)> _viewSizeDispatcher {};

    static inline bool _Constructed = false;

    /** @brief Construct a GPU using a backend window */
    GPU(BackendWindow * const window,
            FrameImageModels &&frameImageModels,
            const std::initializer_list<RenderPassFactory> &renderPassFactories,
            const Core::Version applicationVersion = Core::Version(0, 1, 0)) noexcept;
};

static_assert_alignof_double_cacheline(kF::GPU::GPU);
