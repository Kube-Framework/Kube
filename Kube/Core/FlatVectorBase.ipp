/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVectorBase
 */

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
inline const Type *kF::Core::Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>::
        data(void) const noexcept
{
    if (_ptr) [[likely]]
        return dataUnsafe();
    else
        return nullptr;
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
inline void kF::Core::Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>::
        steal(FlatVectorBase &other) noexcept
{
    std::swap(_ptr, other._ptr);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
inline Type *kF::Core::Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>::
        allocate(const Range capacity) noexcept
{
    auto ptr = reinterpret_cast<Header *>(Allocator::Allocate(sizeof(Header) + sizeof(Type) * capacity, alignof(Header)));
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>) {
        if (_ptr)
            new (&ptr->customType) CustomHeaderType(std::move(_ptr->customType));
        else
            new (&ptr->customType) CustomHeaderType {};
    }
    return reinterpret_cast<Type *>(ptr + 1);
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator, typename CustomHeaderType, std::integral Range>
inline void kF::Core::Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>::
        deallocate(Type * const data, const Range capacity) noexcept
{
    auto ptr = reinterpret_cast<Header *>(data) - 1;
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>)
        ptr->customType.~CustomHeaderType();
    Allocator::Deallocate(ptr, sizeof(Header) + sizeof(Type) * capacity, alignof(Header));
}
