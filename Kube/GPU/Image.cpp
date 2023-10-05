/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Image
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Image.hpp"

using namespace kF;

GPU::Image::~Image(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyImage(parent().logicalDevice(), handle(), nullptr);
}

GPU::Image::Image(const ImageModel &model) noexcept
{
    if (const auto res = ::vkCreateImage(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Image: Couldn't create image '", ErrorMessage(res), '\'');
}