/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread safe static allocator
 */

#pragma once

#include "SafeAllocator.hpp"
#include "StaticAllocator.hpp"

namespace kF::Core
{
    /** @brief Wrapper used to create static safe allocators */
    template<FixedString Name, std::size_t ...Args>
    using StaticSafeAllocator = StaticAllocator<SafeAllocator<Args...>, Name>;
}