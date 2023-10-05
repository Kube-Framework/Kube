/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread safe allocator
 */

#include "SafeAllocator.hpp"

void kF::Core::AllocatorUtils::DestroySafeAllocator(const std::size_t pageSize, SafeStackMetaData * const stack) noexcept
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