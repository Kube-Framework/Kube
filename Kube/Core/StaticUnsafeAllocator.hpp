/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread unsafe static allocator
 */

#pragma once

#include "UnsafeAllocator.hpp"
#include "StaticAllocator.hpp"

namespace kF::Core
{
    /** @brief Wrapper used to create static unsafe allocators */
    template<FixedString Name, std::size_t ...Args>
    using StaticUnsafeAllocator = StaticAllocator<UnsafeAllocator<Args...>, Name>;
}