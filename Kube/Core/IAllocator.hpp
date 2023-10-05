/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocator base class
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    class IAllocator;
}

/** @brief Base class of any allocator */
class kF::Core::IAllocator
{
public:
    /** @brief Virtual destructor */
    virtual ~IAllocator(void) noexcept = default;


    /** @brief Allocate memory */
    [[nodiscard]] virtual void *allocate(const std::size_t size, const std::size_t alignment) noexcept = 0;

    /** @brief Deallocate memory */
    virtual void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept = 0;
};