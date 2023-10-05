/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Opaque event
 */

#pragma once

#include <Kube/Core/Functor.hpp>

#include "Base.hpp"

namespace kF::ECS
{
    /** @brief Opaque pipeline event */
    using PipelineEvent = Core::Functor<void(void), ECSAllocator>;
}