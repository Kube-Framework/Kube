/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PerFrameCache
 */

#include "PerFrameCache.hpp"

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::GPU::PerFrameCache<Type, Allocator>::resize(const FrameIndex count) noexcept
{
    reserve(count);
    std::uninitialized_value_construct_n(_data, count);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::GPU::PerFrameCache<Type, Allocator>::resize(const FrameIndex count, const Type &value) noexcept
{
    reserve(count);
    std::uninitialized_fill_n(_data, count, value);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Initializer>
    requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, kF::GPU::FrameIndex>
inline void kF::GPU::PerFrameCache<Type, Allocator>::resize(const FrameIndex count, Initializer &&initializer) noexcept
{
    reserve(count);
    for (FrameIndex i = 0; i != count; ++i) {
        if constexpr (std::is_invocable_v<Initializer, FrameIndex>)
            new (_data + i) Type(initializer(i));
        else
            new (_data + i) Type(initializer());
    }
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
template<bool ResetMembers>
inline void kF::GPU::PerFrameCache<Type, Allocator>::release(void) noexcept
{
    if (_data) {
        std::destroy_n(_data, _capacity);
        Core::AlignedFree(_data, _capacity * sizeof(Type), alignof(Type));
        if constexpr (ResetMembers) {
            _data = nullptr;
            _capacity = 0;
            _index = 0;
        }
    }
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::GPU::PerFrameCache<Type, Allocator>::reserve(const FrameIndex count) noexcept
{
    release();
    _data = Core::AlignedAlloc<Type>(count * sizeof(Type), alignof(Type));
    _capacity = count;
    _index = 0;
}
