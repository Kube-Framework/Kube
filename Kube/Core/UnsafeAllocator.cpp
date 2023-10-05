/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread unsafe allocator
 */

#include "UnsafeAllocator.hpp"

void kF::Core::AllocatorUtils::Internal::DestroyUnsafeAllocator(const std::size_t pageSize, UnsafeStackMetaData * const stack) noexcept
{
    if (!stack) [[unlikely]]
        return;

    // Reverse linked list
    auto prev = stack;
    auto it = prev->next;
    prev->next = nullptr;
    while (it) [[likely]] {
        auto next = it->next;
        it->next = prev;
        prev = it;
        it = next;
    }

    // Destroy each stack
    it = prev;
    while (it) {
        auto next = it->next;
        AllocatorUtils::FallbackDeallocate(it, it->size, pageSize);
        it = next;
    }
}