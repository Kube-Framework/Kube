/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow base
 */

#pragma once

#include <Kube/Core/StaticSafeAllocator.hpp>

namespace kF::Flow
{
    /** @brief Allocator of the flow library */
    struct FlowAllocator : Core::StaticSafeAllocator<"FlowAllocator"> {};


    /** @brief Sleep as precisely as possible to the given nanosecond duration */
    void PreciseSleep(const std::int64_t nanoseconds) noexcept;
}