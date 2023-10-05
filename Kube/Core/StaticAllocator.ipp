/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Static allocator wrapper
 */

#include "StaticAllocator.hpp"

template<kF::Core::AllocatorRequirements Allocator, kF::Core::FixedString Name>
inline void *kF::Core::StaticAllocator<Allocator, Name>::Allocate(const std::size_t bytes, const std::size_t alignment) noexcept
{
    if (_Instance.allocator) [[likely]] {
AllocateLabel:
        return _Instance.allocator->allocate(bytes, alignment);
    } else [[unlikely]] {
        _Instance.allocator = new Allocator();
        goto AllocateLabel;
    }
}

template<kF::Core::AllocatorRequirements Allocator, kF::Core::FixedString Name>
inline void kF::Core::StaticAllocator<Allocator, Name>::Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
{
    if (data) [[likely]] {
        _Instance.allocator->deallocate(data, bytes, alignment);
        if (_Instance.destroyPending) [[unlikely]]
            EnsureDestruction();
    }
}

template<kF::Core::AllocatorRequirements Allocator, kF::Core::FixedString Name>
no_inline void kF::Core::StaticAllocator<Allocator, Name>::EnsureDestruction(void) noexcept
{
    if (_Instance.allocator->empty()) [[unlikely]]
        _Instance.destroyInstance();
}

template<kF::Core::AllocatorRequirements Allocator>
inline kF::Core::Internal::StaticAllocatorInstance<Allocator>::~StaticAllocatorInstance(void) noexcept
{
    if (allocator && allocator->empty()) [[unlikely]] {
        destroyInstance();
    } else
        destroyPending = true;
}

template<kF::Core::AllocatorRequirements Allocator>
inline void kF::Core::Internal::StaticAllocatorInstance<Allocator>::destroyInstance(void) noexcept
{
    delete allocator;
    allocator = nullptr;
    destroyPending = false;
}
