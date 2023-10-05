/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Swapchain
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    class Swapchain;
}

/** @brief Abstraction of renderer swapchain */
class alignas_cacheline kF::GPU::Swapchain : public CachedHandle<SwapchainHandle>
{
public:
    /** @brief Maximum amount of cached frames */
    static constexpr std::uint32_t MaxCachedFrame = 2u;

    /** @brief Swapchain image handles */
    using SwapchainImages = Core::Vector<ImageHandle, GPUAllocator>;


    /** @brief Destrut the swapchain */
    ~Swapchain(void) noexcept;

    /** @brief Construct the swapchain */
    Swapchain(void) noexcept;

    /** @brief Move constructor */
    Swapchain(Swapchain &&other) noexcept = default;

    /** @brief Move assignment */
    Swapchain &operator=(Swapchain &&other) noexcept { swap(other); return *this; }


    /** @brief Get the surface format */
    [[nodiscard]] SurfaceFormat &surfaceFormat(void) noexcept { return _surfaceFormat; }
    [[nodiscard]] const SurfaceFormat &surfaceFormat(void) const noexcept { return _surfaceFormat; }

    /** @brief Get the surface presentationmode */
    [[nodiscard]] PresentMode &presentMode(void) noexcept { return _presentMode; }
    [[nodiscard]] const PresentMode &presentMode(void) const noexcept { return _presentMode; }

    /** @brief Get the extent */
    [[nodiscard]] Extent2D &extent(void) noexcept { return _extent; }
    [[nodiscard]] const Extent2D &extent(void) const noexcept { return _extent; }


    /** @brief Query swapchain images */
    [[nodiscard]] SwapchainImages queryImages(void) const noexcept;


    /** @brief Callback on render view size changed */
    void onViewSizeChanged(void) noexcept;

private:
    Extent2D _extent {};
    SurfaceFormat _surfaceFormat {};
    PresentMode _presentMode {};

    /** @brief Create the swapchain */
    void createSwapchain(const SwapchainHandle oldSwapchain) noexcept;
};

static_assert_fit_cacheline(kF::GPU::Swapchain);
