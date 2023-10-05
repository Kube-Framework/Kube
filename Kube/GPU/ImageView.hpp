/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageView.hpp
 */

#pragma once

#include "Handle.hpp"
#include "ImageViewModel.hpp"

namespace kF::GPU
{
    class ImageView;
};

/** @brief Abstract an image view */
class kF::GPU::ImageView : public Handle<ImageViewHandle>
{
public:
    /** @brief Destruct the buffer */
    ~ImageView(void) noexcept;

    /** @brief Default constructor */
    ImageView(void) noexcept = default;

    /** @brief Construct a new ImageView using ImageView model */
    ImageView(const ImageViewModel &model) noexcept;

    /** @brief Move constructor */
    ImageView(ImageView &&other) noexcept = default;

    /** @brief Move assignment */
    ImageView &operator=(ImageView &&other) noexcept = default;
};