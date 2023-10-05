/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "Functor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with 'Functor' */
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
            std::size_t DesiredSize = CacheLineHalfSize>
    using Dispatcher = DispatcherDetails<Signature, Functor<Signature, Allocator, DesiredSize>, Allocator>;
}