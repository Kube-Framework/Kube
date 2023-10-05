/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocated Small Vector
 */

template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
inline Type *kF::Core::Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, Range>::
        allocate(const Range capacity) noexcept
{
    if (capacity <= OptimizedCapacity) [[likely]]
        return optimizedData();
    else
        return reinterpret_cast<Type *>(_allocator->allocate(sizeof(Type) * capacity, alignof(Type)));
}

template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
inline void kF::Core::Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, Range>::
        deallocate(Type * const data, const Range capacity) noexcept
{
    if (data != optimizedData()) [[unlikely]]
        _allocator->deallocate(data, sizeof(Type) * capacity, alignof(Type));
}