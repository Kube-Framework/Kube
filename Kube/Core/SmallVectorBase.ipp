/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
inline void kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>::
        steal(SmallVectorBase &other) noexcept
{
    if (_data) {
        std::destroy(beginUnsafe(), endUnsafe());
        deallocate(_data, _capacity);
    }
    if (other.isCacheUsed()) {
        std::uninitialized_move(other.beginUnsafe(), other.endUnsafe(), optimizedData());
        _data = optimizedData();
    } else {
        _data = other._data;
    }
    _size = other._size;
    _capacity = other._capacity;
    other._data = nullptr;
    other._size = Range {};
    other._capacity = Range {};
}

template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
inline void kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>::
        swap(SmallVectorBase &other) noexcept
{
    const auto begin = beginUnsafe();
    const auto otherBegin = other.beginUnsafe();

    if (other.isCacheUsed()) {
        if (isCacheUsed()) {
            const auto size = sizeUnsafe();
            const auto otherSize = other.sizeUnsafe();
            if (size < otherSize) {
                std::swap_ranges(begin, endUnsafe(), otherBegin);
                std::uninitialized_move(begin + (otherSize - size), endUnsafe(), otherBegin + size);
            } else if (size > otherSize) {
                std::swap_ranges(otherBegin, other.endUnsafe(), otherBegin);
                std::uninitialized_move(otherBegin + (otherSize - size), other.endUnsafe(), begin + otherSize);
            } else {
                std::swap_ranges(begin, endUnsafe(), otherBegin);
            }
        } else {
            std::uninitialized_move(other.begin(), other.end(), optimizedData());
            other._data = _data;
            _data = optimizedData();
        }
    } else if (isCacheUsed()) {
        std::uninitialized_move(begin, endUnsafe(), other.optimizedData());
        _data = other._data;
        other._data = other.optimizedData();
    } else {
        std::swap(_data, other._data);
    }
    std::swap(_size, other._size);
    std::swap(_capacity, other._capacity);
}

template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
inline Type *kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>::
        allocate(const Range capacity) noexcept
{
    if (capacity <= OptimizedCapacity) [[likely]]
        return optimizedData();
    else
        return reinterpret_cast<Type *>(Allocator::Allocate(sizeof(Type) * capacity, alignof(Type)));
}

template<typename Type, std::size_t OptimizedCapacity, kF::Core::StaticAllocatorRequirements Allocator, std::integral Range>
inline void kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>::
        deallocate(Type * const data, const Range capacity) noexcept
{
    if (data != optimizedData()) [[unlikely]]
        Allocator::Deallocate(data, sizeof(Type) * capacity, alignof(Type));
}
