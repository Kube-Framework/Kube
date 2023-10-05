/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ImageView
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "ImageView.hpp"

using namespace kF;

GPU::ImageView::~ImageView(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyImageView(parent().logicalDevice(), handle(), nullptr);
}

GPU::ImageView::ImageView(const ImageViewModel &model) noexcept
{
    if (const auto res = ::vkCreateImageView(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::ImageView: Couldn't create image view '", ErrorMessage(res), '\'');
}