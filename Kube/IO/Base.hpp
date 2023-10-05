/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO Base
 */

#pragma once

#include <Kube/Core/StaticSafeAllocator.hpp>

namespace kF::IO
{
    /** @brief Allocator of the IO library */
    using IOAllocator = Core::DefaultStaticAllocator;


    /** @brief Prefix name of any resource */
    constexpr std::string_view ResourcePrefix = ":/";

    /** @brief Index of the environment name */
    constexpr std::uint32_t EnvironmentBeginIndex = ResourcePrefix.size();


    /** @brief View of a resource */
    using ResourceView = Core::IteratorRange<const std::uint8_t *>;
}