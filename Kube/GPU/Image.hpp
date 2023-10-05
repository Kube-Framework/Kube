/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Image.hpp
 */

#pragma once

#include "Handle.hpp"
#include "ImageModel.hpp"

namespace kF::GPU
{
    class Image;
};

/** @brief Abstract an image */
class kF::GPU::Image : public Handle<ImageHandle>
{
public:
    /** @brief Helper used to create a single layer 2D image */
    [[nodiscard]] static Image MakeSingleLayer2D(const Extent2D &extent, const Format format, const ImageUsageFlags customFlags, const ImageTiling tiling) noexcept
        { return Image(ImageModel::MakeSingleLayer2D(extent, format, customFlags, tiling)); }

    /** @brief Helper used to create a depth image */
    [[nodiscard]] static Image MakeDepthStencil(const Extent2D &extent, const Format format, const ImageUsageFlags customFlags = ImageUsageFlags::None) noexcept
        { return Image(ImageModel::MakeDepthStencil(extent, format, customFlags)); }


    /** @brief Destruct the buffer */
    ~Image(void) noexcept;

    /** @brief Default constructor */
    Image(void) noexcept = default;

    /** @brief Construct a new Image using image model */
    Image(const ImageModel &model) noexcept;

    /** @brief Move constructor */
    Image(Image &&other) noexcept = default;

    /** @brief Move assignment */
    Image &operator=(Image &&other) noexcept = default;
};