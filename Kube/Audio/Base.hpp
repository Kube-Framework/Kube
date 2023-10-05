/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio base definitions
 */

#pragma once

#include <Kube/Core/StaticSafeAllocator.hpp>

namespace kF::Audio
{
    /** @brief Allocator of the audio library */
    using AudioAllocator = Core::DefaultStaticAllocator;
}