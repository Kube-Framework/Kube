/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: HeapArray
 */

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
template<typename ...Args>
    requires std::constructible_from<Type, Args...>
inline void kF::Core::HeapArray<Type, Allocator, Range>::allocateUnsafe(const Range size, Args ...args) noexcept
{
    _data = reinterpret_cast<Type *>(Allocator::Allocate(sizeof(Type) * size, alignof(Type)));
    _size = size;
    for (auto i = static_cast<Range>(0); i != size; ++i)
        new (&_data[i]) Type(args...);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
inline void kF::Core::HeapArray<Type, Allocator, Range>::release(void) noexcept
{
    if (!_data)
        return;
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        for (auto &elem : *this)
            elem.~Type();
    }
    Allocator::Deallocate(_data, sizeof(Type) * _size, alignof(Type));
    _data = nullptr;
    _size = static_cast<Range>(0);
}
