/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sampler
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Sampler.hpp"

using namespace kF;

GPU::Sampler::~Sampler(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroySampler(parent().logicalDevice(), handle(), nullptr);
}

GPU::Sampler::Sampler(const SamplerModel &model) noexcept
{
    if (const auto res = ::vkCreateSampler(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Sampler: Couldn't create sampler '", ErrorMessage(res), '\'');
}