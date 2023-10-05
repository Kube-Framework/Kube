/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Surface
 */

#pragma once

#include "Handle.hpp"

namespace kF::GPU
{
    class Surface;
}

/** @brief Abstraction of a render surface */
class kF::GPU::Surface : public CachedHandle<SurfaceHandle>
{
public:
    /** @brief Destruct the surface */
    ~Surface(void) noexcept;

    /** @brief Construct a surface */
    Surface(void) noexcept;

    /** @brief Move constructor */
    Surface(Surface &&other) noexcept = default;

    /** @brief Move assignment */
    Surface &operator=(Surface &&other) noexcept = default;


    /** @brief Get the surface format */
    [[nodiscard]] SurfaceFormat getSurfaceFormat(void) const noexcept;

    /** @brief Get the presentation mode */
    [[nodiscard]] PresentMode getPresentMode(void) const noexcept;

    /** @brief Get the surface capabilities */
    [[nodiscard]] SurfaceCapabilities getSurfaceCapabilities(void) const noexcept;

    /** @brief Get the extent */
    [[nodiscard]] Extent2D getExtent(const SurfaceCapabilities &capabilities) const noexcept;
};