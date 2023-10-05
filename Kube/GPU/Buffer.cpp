/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Buffer
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Buffer.hpp"

using namespace kF;

GPU::Buffer::~Buffer(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyBuffer(parent().logicalDevice(), handle(), nullptr);
}

GPU::Buffer::Buffer(const BufferModel &model) noexcept
{
    if (const auto res = ::vkCreateBuffer(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Buffer: Couldn't create buffer '", ErrorMessage(res), '\'');
}