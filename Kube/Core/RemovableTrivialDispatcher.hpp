/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "TrivialFunctor.hpp"
#include "RemovableDispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of removable dispatcher with 'TrivialFunctor' */
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
            std::size_t DesiredSize = CacheLineQuarterSize>
    using RemovableTrivialDispatcher = RemovableDispatcherDetails<Signature, TrivialFunctor<Signature, DesiredSize>, Allocator>;
}