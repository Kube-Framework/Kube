/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String details
 */

#include "StringDetails.hpp"

template<std::integral Range, typename Type>
inline Range kF::Core::Internal::SafeStrlen(const Type * const cstring) noexcept
{
    if (!cstring)
        return Range {};
    else
        return static_cast<Range>(std::strlen(cstring));
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const Type lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept
{
    kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> res;
    res.reserve(rhs.size() + 1);
    res.push(lhs);
    res.insert(res.end(), rhs.begin(), rhs.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const Type * const lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept
{
    kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> res;
    const auto length = kF::Core::Internal::SafeStrlen<Range>(lhs);
    res.reserve(length + rhs.size());
    res.insert(res.end(), lhs, lhs + length);
    res.insert(res.end(), rhs.begin(), rhs.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> operator+(
    const typename kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::View &lhs,
    const kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> &rhs
) noexcept
{
    kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> res;
    res.reserve(rhs.size() + static_cast<Range>(lhs.size()));
    res.insert(res.end(), lhs.begin(), lhs.end());
    res.insert(res.end(), rhs.begin(), rhs.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const StringDetails &other) const noexcept
{
    StringDetails res;
    res.reserve(size() + other.size());
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const Type character) const noexcept
{
    StringDetails res;
    res.reserve(size() + 1);
    res.insert(res.end(), begin(), end());
    res.push(character);
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const char * const cstring) const noexcept
{
    StringDetails res;
    const auto length = SafeStrlen<Range>(cstring);
    res.reserve(size() + length);
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), cstring, cstring + length);
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivially_copyable_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const View &other) const noexcept
{
    StringDetails res;
    res.reserve(size() + static_cast<Range>(other.size()));
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}