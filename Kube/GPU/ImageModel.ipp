/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageModel
 */

#include "ImageModel.hpp"

inline kF::GPU::ImageModel kF::GPU::ImageModel::MakeSingleLayer2D(
        const Extent2D &extent, const Format format, const ImageUsageFlags customFlags, const ImageTiling tiling) noexcept
{
    return ImageModel(
        ImageCreateFlags::None,
        ImageType::Type2D,
        format,
        Extent3D {
            .width = extent.width,
            .height = extent.height,
            .depth = 1
        },
        1,
        1,
        SampleCountFlags::X1,
        tiling,
        customFlags,
        SharingMode::Exclusive,
        nullptr, nullptr,
        ImageLayout::Undefined
    );
}

inline kF::GPU::ImageModel kF::GPU::ImageModel::MakeDepthStencil(const Extent2D &extent, const Format format, const ImageUsageFlags customFlags) noexcept
{
    return MakeSingleLayer2D(
        extent,
        format,
        Core::MakeFlags(ImageUsageFlags::DepthStencilAttachment, customFlags),
        ImageTiling::TilingOptimal
    );
}