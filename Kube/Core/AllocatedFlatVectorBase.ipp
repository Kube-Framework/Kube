/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedFlatVectorBase
 */

#include "AllocatedFlatVectorBase.hpp"

template<typename Type, typename CustomHeaderType, std::integral Range>
inline Type *kF::Core::Internal::AllocatedFlatVectorBase<Type, CustomHeaderType, Range>::
        allocate(const Range capacity) noexcept
{
    auto ptr = reinterpret_cast<Header *>(_allocator->allocate(sizeof(Header) + sizeof(Type) * capacity, alignof(Header)));
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>) {
        if (_ptr)
            new (&ptr->customType) CustomHeaderType(std::move(_ptr->customType));
        else
            new (&ptr->customType) CustomHeaderType {};
    }
    return reinterpret_cast<Type *>(ptr + 1);
}

template<typename Type, typename CustomHeaderType, std::integral Range>
inline void kF::Core::Internal::AllocatedFlatVectorBase<Type, CustomHeaderType, Range>::
        deallocate(Type * const data, const Range capacity) noexcept
{
    auto ptr = reinterpret_cast<Header *>(data) - 1;
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>)
        ptr->customType.~CustomHeaderType();
    _allocator->deallocate(ptr, sizeof(Header) + sizeof(Type) * capacity, alignof(Header));
}