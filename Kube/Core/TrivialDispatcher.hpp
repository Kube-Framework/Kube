/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: TrivialDispatcher
 */

#pragma once

#include "TrivialFunctor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with 'TrivialFunctor' */
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
            std::size_t DesiredSize = CacheLineQuarterSize>
    using TrivialDispatcher = DispatcherDetails<Signature, TrivialFunctor<Signature, DesiredSize>, Allocator>;
}