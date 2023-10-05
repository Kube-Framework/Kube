/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Static allocator wrapper
 */

#pragma once

#include "FixedString.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    template<kF::Core::AllocatorRequirements Allocator, kF::Core::FixedString Name>
    class StaticAllocator;

    namespace Internal
    {
        /** @brief Static instance */
        template<kF::Core::AllocatorRequirements Allocator>
        struct StaticAllocatorInstance
        {
            Allocator *allocator {};
            bool destroyPending {};


            /** @brief Destructor */
            ~StaticAllocatorInstance(void) noexcept;

            /** @brief Manual instance destruction */
            void destroyInstance(void) noexcept;
        };
    }
}

/** @brief Wrapper used to create static allocators */
template<kF::Core::AllocatorRequirements Allocator, kF::Core::FixedString Name>
class kF::Core::StaticAllocator
{
public:
    /** @brief Allocate function that forward to AlignedAlloc */
    [[nodiscard]] static void *Allocate(const std::size_t bytes, const std::size_t alignment) noexcept;

    /** @brief Deallocate function that forward to AlignedFree */
    static void Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept;

private:
    /** @brief Ensure destruction of the allocator when destruction is pending */
    static void EnsureDestruction(void) noexcept;

    static inline Internal::StaticAllocatorInstance<Allocator> _Instance {};
};

#include "StaticAllocator.ipp"