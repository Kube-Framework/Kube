/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "Functor.hpp"
#include "RemovableDispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of removable dispatcher with 'Functor' */
    template<
        typename Signature,
        StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
        std::size_t DesiredSize = CacheLineHalfSize
    >
    using RemovableDispatcher = RemovableDispatcherDetails<Signature, Functor<Signature, Allocator, DesiredSize>, Allocator>;
}