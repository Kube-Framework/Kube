/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of utility used globally in the framework
 */

#pragma once

#include <type_traits>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <cstdint>

#include "Platform.hpp"

/** @brief Helper used to pass template into macro */
#define TEMPLATE_TYPE(Class, ...) decltype(std::declval<Class<__VA_ARGS__>>())


/** @brief Align a variable / structure to cacheline size */
#define alignas_cacheline alignas(kF::Core::CacheLineSize)
#define alignas_double_cacheline alignas(kF::Core::CacheLineDoubleSize)
#define alignas_half_cacheline alignas(kF::Core::CacheLineHalfSize)
#define alignas_quarter_cacheline alignas(kF::Core::CacheLineQuarterSize)
#define alignas_eighth_cacheline alignas(kF::Core::CacheLineEighthSize)


/** @brief Helpers used to assert alignment of a structure */
#define static_assert_alignof(Type, Alignment) static_assert(alignof(Type) == Alignment, #Type " must be aligned to " #Alignment)
#define static_assert_alignof_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineSize)
#define static_assert_alignof_double_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_alignof_half_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineHalfSize)
#define static_assert_alignof_quarter_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_alignof_eighth_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineEighthSize)

/** @brief Helpers used to assert size of a structure */
#define static_assert_sizeof(Type, Size) static_assert(sizeof(Type) == Size, #Type " must have a size of " #Size)
#define static_assert_sizeof_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineSize)
#define static_assert_sizeof_double_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_sizeof_half_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineHalfSize)
#define static_assert_sizeof_quarter_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_sizeof_eighth_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineEighthSize)

/** @brief Helpers used to assert that the size and alignment of a structure are equal to themselves and a given value */
#define static_assert_fit(Type, Size) static_assert(sizeof(Type) == alignof(Type) && alignof(Type) == Size, #Type " must have a size of " #Size " and be aligned to " #Size)
#define static_assert_fit_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineSize)
#define static_assert_fit_double_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_fit_half_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineHalfSize)
#define static_assert_fit_quarter_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_fit_eighth_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineEighthSize)


/** @brief Compile-time ternary expression */
#define ConstexprTernary(condition, body, elseBody) [] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime reference capture */
#define ConstexprTernaryRef(condition, body, elseBody) [&] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime copy capture */
#define ConstexprTernaryCopy(condition, body, elseBody) [=] { if constexpr (condition) { return body; } else { return elseBody; } }()


/** @brief Always inline hint (works only on GCC) */
#if KUBE_COMPILER_GCC | KUBE_COMPILER_CLANG
# define force_inline inline __attribute__((always_inline))
# define no_inline __attribute__((noinline))
#else
# define force_inline inline
# define no_inline __declspec(noinline)
#endif

namespace kF::Core
{
    /** @brief Version structure */
    struct Version
    {
        std::uint8_t major {};
        std::uint8_t minor {};
        std::uint8_t patch {};

        /** @brief Comparison operators */
        [[nodiscard]] constexpr bool operator==(const Version &other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const Version &other) const noexcept = default;

        /** @brief Check if version is inferior */
        [[nodiscard]] constexpr bool operator<(const Version &other) const noexcept
        {
            if (major != other.major)
                return major < other.major;
            else if (minor != other.minor)
                return minor < other.minor;
            else
                return patch < other.patch;
        }
    };

    /** @brief Kube version*/
    constexpr Version KubeVersion {
        .major = 0,
        .minor = 1,
        .patch = 0
    };


    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = sizeof(std::size_t) * 8;
    constexpr std::size_t CacheLineDoubleSize = CacheLineSize * 2;
    constexpr std::size_t CacheLineHalfSize = CacheLineSize / 2;
    constexpr std::size_t CacheLineQuarterSize = CacheLineSize / 4;
    constexpr std::size_t CacheLineEighthSize = CacheLineSize / 8;

    /** @brief Pi constant */
    constexpr auto Pi = 3.14159265358979323846;


    /** @brief Get distance converted into UnitType */
    template<typename UnitType, typename Iterator>
    [[nodiscard]] constexpr UnitType Distance(const Iterator from, const Iterator to) noexcept
        { return static_cast<UnitType>(std::distance(from, to)); }

    /** @brief Similar to std::aligned_alloc, but ensure arguments, you must use AlignedFree to free the memory */
    template<typename Cast = void>
    [[nodiscard]] inline Cast *AlignedAlloc(const std::size_t bytes, const std::size_t alignment) noexcept
        { return reinterpret_cast<Cast *>(::operator new(bytes, static_cast<std::align_val_t>(alignment), std::nothrow)); }

    /** @brief Free a pointer allocated with AlignedAlloc */
    inline void AlignedFree(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
        { ::operator delete(data, bytes, static_cast<std::align_val_t>(alignment)); }


    /** @brief Concept of an allocator */
    template<typename Type>
    concept AllocatorRequirements = requires(Type &allocator, void *data, std::size_t bytes, std::size_t alignment)
    {
        { allocator.allocate(bytes, alignment) } -> std::same_as<void *>;
        { allocator.deallocate(data, bytes, alignment) } -> std::same_as<void>;
        { allocator.empty() } -> std::same_as<bool>;
    };

    /** @brief Concept of a static allocator */
    template<typename Type>
    concept StaticAllocatorRequirements = requires(void *data, std::size_t bytes, std::size_t alignment)
    {
        { Type::Allocate(bytes, alignment) } -> std::same_as<void *>;
        { Type::Deallocate(data, bytes, alignment) } -> std::same_as<void>;
    };

    /** @brief Default static allocator */
    struct DefaultStaticAllocator
    {
        /** @brief Allocate function that forward to AlignedAlloc */
        [[nodiscard]] static inline void *Allocate(const std::size_t bytes, const std::size_t alignment) noexcept
            { return AlignedAlloc(bytes, alignment); }

        /** @brief Deallocate function that forward to AlignedFree */
        static inline void Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
            { return AlignedFree(data, bytes, alignment); }
    };
    static_assert(StaticAllocatorRequirements<DefaultStaticAllocator>, "Default static allocator doesn't meet requirements");

    /** @brief Reference wrapper */
    template<typename Type>
    class Ref
    {
    public:
        /** @brief Destructor */
        constexpr ~Ref(void) noexcept = default;

        /** @brief Default constructor */
        constexpr Ref(void) noexcept = default;

        /** @brief Copy constructor */
        constexpr Ref(const Ref &other) noexcept = default;

        /** @brief Data pointer constructor */
        constexpr Ref(Type * const data) noexcept : _data(data) {}

        /** @brief Data reference constructor */
        constexpr Ref(Type &data) noexcept : _data(&data) {}

        /** @brief Copy constructor */
        constexpr Ref &operator=(const Ref &other) noexcept = default;

        /** @brief Data pointer constructor */
        constexpr Ref &operator=(Type * const data) noexcept { _data = data; return *this; }

        /** @brief Data reference constructor */
        constexpr Ref &operator=(Type &data) noexcept { _data = &data; return *this; }

        /** @brief Boolean operator */
        [[nodiscard]] explicit constexpr operator bool(void) const noexcept { return _data; }

        /** @brief Get data */
        [[nodiscard]] constexpr Type &get(void) const noexcept { return *_data; }
        [[nodiscard]] constexpr operator Type *(void) const noexcept { return _data; }
        [[nodiscard]] constexpr operator Type &(void) const noexcept { return *_data; }
        [[nodiscard]] constexpr Type *operator->(void) const noexcept { return _data; }
        [[nodiscard]] constexpr Type &operator*(void) const noexcept { return *_data; }

        /** @brief Comparison operators */
        template<typename Other>
        [[nodiscard]] constexpr bool operator==(const Ref &other) const noexcept { return _data == other; }
        template<typename Other>
        [[nodiscard]] constexpr bool operator!=(const Ref &other) const noexcept { return _data != other; }

    private:
        Type *_data {};
    };

    /** @brief Simple pair of random access begin / end iterators */
    template<std::random_access_iterator Iterator_>
    struct IteratorRange
    {
        /** @brief Iterator */
        using Iterator = Iterator_;

        /** @brief Reverse iterator */
        using ReverseIterator = std::reverse_iterator<Iterator>;

        /** @brief Underlying type */
        using Type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;

        Iterator from {};
        Iterator to {};

        /** @brief Conversion operator */
        [[nodiscard]] inline operator IteratorRange<std::add_const_t<Type> *>(void) const noexcept
            { return { from, to }; }

        /** @brief Empty check */
        [[nodiscard]] inline bool empty(void) const noexcept { return from == to; }

        /** @brief Get data */
        [[nodiscard]] inline Iterator data(void) const noexcept { return from; }

        /** @brief Begin / End iterators */
        [[nodiscard]] inline Iterator begin(void) const noexcept { return from; }
        [[nodiscard]] inline Iterator end(void) const noexcept { return to; }

        /** @brief Reverse Begin / End iterators */
        [[nodiscard]] inline ReverseIterator rbegin(void) const noexcept { return std::make_reverse_iterator(to); }
        [[nodiscard]] inline ReverseIterator rend(void) const noexcept { return std::make_reverse_iterator(from); }

        /** @brief Range size */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline Range size(void) const noexcept { return static_cast<Range>(std::distance(from, to)); }

        /** @brief Dereference element at */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline auto &operator[](const Range index) const noexcept { return from[index]; }

        /** @brief Dereference element at */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline auto &at(const Range index) const noexcept { return from[index]; }

        /** @brief Get first element */
        [[nodiscard]] inline Type &front(void) noexcept { return at(0); }
        [[nodiscard]] inline const Type &front(void) const noexcept { return at(0); }

        /** @brief Get last element */
        [[nodiscard]] inline Type &back(void) noexcept { return at(size() - 1); }
        [[nodiscard]] inline const Type &back(void) const noexcept { return at(size() - 1); }

        /** @brief Comparison operators */
        [[nodiscard]] inline bool operator==(const IteratorRange &other) const noexcept
            { return (from == other.from) & (to == other.to); }
        [[nodiscard]] inline bool operator!=(const IteratorRange &other) const noexcept
            { return (from != other.from) | (to != other.to); }

        /** @brief Get a subrange with an offset */
        template<typename Range>
        [[nodiscard]] inline IteratorRange subrange(const Range offset) const noexcept
            { return IteratorRange { from + offset, to }; }

        /** @brief Get a subrange with an offset and a count */
        template<typename Range>
        [[nodiscard]] inline IteratorRange subrange(const Range offset, const Range count) const noexcept
            { return IteratorRange { from + offset, from + offset + count }; }


        /** @brief Find an element by comparison */
        template<typename Comparable>
            requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
        [[nodiscard]] inline Iterator find(const Comparable &comparable) const noexcept
            { return std::find(begin(), end(), comparable); }

        /** @brief Find an element by comparison, using begin iterator */
        template<typename Comparable>
            requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
        [[nodiscard]] inline Iterator find(const Iterator from, const Comparable &comparable) const noexcept
            { return std::find(from, end(), comparable); }

        /** @brief Find an element by comparison with reverse order */
        template<typename Comparable>
            requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
        [[nodiscard]] inline ReverseIterator rfind(const Comparable &comparable) const noexcept
            { return std::find(rbegin(), rend(), comparable); }

        /** @brief Find an element by comparison with reverse order, using begin iterator */
        template<typename Comparable>
            requires requires(const Type &lhs, const Comparable &rhs) { lhs == rhs; }
        [[nodiscard]] inline ReverseIterator rfind(const ReverseIterator from, const Comparable &comparable) const noexcept
            { return std::find(from, rend(), comparable); }

        /** @brief Find an element with functor */
        template<typename Functor>
            requires std::invocable<Functor, const Type &>
        [[nodiscard]] inline Iterator find(Functor &&functor) const noexcept
            { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }

        /** @brief Find an element with functor, using begin iterator */
        template<typename Functor>
            requires std::invocable<Functor, const Type &>
        [[nodiscard]] inline Iterator find(const Iterator from, Functor &&functor) const noexcept
            { return std::find_if(from, end(), std::forward<Functor>(functor)); }

        /** @brief Find an element with functor with reverse order */
        template<typename Functor>
            requires std::invocable<Functor, const Type &>
        [[nodiscard]] inline ReverseIterator rfind(Functor &&functor) const noexcept
            { return std::find_if(rbegin(), rend(), std::forward<Functor>(functor)); }

        /** @brief Find an element with functor with reverse order, using reversed begin iterator */
        template<typename Functor>
            requires std::invocable<Functor, const Type &>
        [[nodiscard]] inline ReverseIterator rfind(const ReverseIterator from, Functor &&functor) const noexcept
            { return std::find_if(from, rend(), std::forward<Functor>(functor)); }
    };

    /** @brief Deduction guide for iterator range */
    template<typename Iterator>
    IteratorRange(const Iterator from, const Iterator to) -> IteratorRange<Iterator>;


    /** @brief Convert any container to an iterator range */
    template<typename Container>
        requires requires(Container &container) { std::begin(container); std::end(container); }
    constexpr auto MakeRange(Container &container) noexcept
        { return Core::IteratorRange<decltype(std::begin(container))> { std::begin(container), std::end(container) }; }

    /** @brief Make an iterator range out of two iterators */
    template<typename Iterator>
    constexpr auto MakeRange(const Iterator from, const Iterator to) noexcept
        { return Core::IteratorRange<std::remove_cvref_t<Iterator>> { from, to }; }


    /** @brief Helper to know if a given type is a std::move_iterator */
    template<typename Type>
    struct IsMoveIterator;

    /** @brief Helper that match non-move iterators */
    template<typename Type>
    struct IsMoveIterator
    {
        static constexpr bool Value = false;
    };

    /** @brief Helper that match move iterators */
    template<typename Iterator>
    struct IsMoveIterator<std::move_iterator<Iterator>> : public IsMoveIterator<Iterator>
    {
        static constexpr bool Value = true;
    };

    /** @brief Helper that match reverse iterators */
    template<typename Iterator>
    struct IsMoveIterator<std::reverse_iterator<Iterator>> : public IsMoveIterator<Iterator>
    {};


    /** @brief Dummy type used to compile unions at constexpr with a single used type */
    struct DummyType
    {
        /** @brief Dummy constructor */
        constexpr DummyType(void) noexcept {}

        /** @brief Comparison operators */
        [[nodiscard]] constexpr bool operator==(const DummyType &other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const DummyType &other) const noexcept = default;
    };

    namespace Internal
    {
        /** @brief Helper to know if a given type is dereferencable - Failure case */
        template <typename>
        std::false_type Dereferencable(unsigned long) noexcept;

        /** @brief Helper to know if a given type is dereferencable - Success case */
        template <typename Type>
        auto Dereferencable(int) noexcept -> decltype(*std::declval<Type>(), std::true_type {});
    }

    /** @brief Helper to know if a given type is dereferencable */
    template<typename Type>
    constexpr bool IsDereferencable = decltype(Internal::Dereferencable<Type>(0))::value;

    template<typename Type>
        requires IsDereferencable<Type>
    using DereferenceType = std::remove_cvref_t<decltype(*std::declval<Type>())>;


    /** @brief Align any offset to a power of 2 alignment */
    template<std::integral Range>
    [[nodiscard]] constexpr Range AlignPowerOf2(const Range offset, const Range alignment) noexcept;

    /** @brief Align any offset to a non-power of 2 alignment */
    template<std::integral Range>
    [[nodiscard]] constexpr Range AlignNonPowerOf2(const Range offset, const Range alignment) noexcept;

    /** @brief Check if value is a power of 2 */
    template<std::integral Unit>
    [[nodiscard]] constexpr Unit IsPowerOf2(Unit value) noexcept;

    /** @brief Find the closest power of 2 of value */
    template<std::integral Unit>
    [[nodiscard]] constexpr Unit NextPowerOf2(Unit value) noexcept;

    /** @brief Get closest power of 2 of value as bit position */
    template<std::integral Unit, std::integral ResultUnit = std::size_t>
    [[nodiscard]] constexpr ResultUnit NextPowerOf2Bit(Unit value) noexcept;


    /** @brief Enum class flag maker */
    template<typename Type, typename ...Args>
        requires (std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr Type MakeFlags(const Type first, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Type>(((static_cast<Underlying>(first) | static_cast<Underlying>(args)) | ...));
    }

    /** @brief Enum class flag maker */
    template<typename Type>
    [[nodiscard]] constexpr Type MakeFlags(const Type flag) noexcept { return flag; }

    /** @brief Enum class flag remover */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr Type RemoveFlags(const Type flags, Args ...toRemove) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Type>((static_cast<Underlying>(flags) & ~static_cast<Underlying>(MakeFlags(toRemove...))));
    }

    /** @brief Enum class flag tester, all flags must be set */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr bool HasFlags(const Type value, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return ((static_cast<Underlying>(value) & static_cast<Underlying>(args)) && ...);
    }

    /** @brief Enum class flag tester, at least one of the variadic flags must be set */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr bool HasAnyFlags(const Type value, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Underlying>(value) & (static_cast<Underlying>(args) | ...);
    }

    /** @brief Get underlying value of an enumeration */
    template<typename Type>
    [[nodiscard]] constexpr auto ToUnderlying(const Type value) noexcept
        { return static_cast<std::underlying_type_t<Type>>(value); }


    /** @brief Branchless computed if
     *  @todo Benchmark */
    template<typename Type>
        requires std::integral<Type> || std::floating_point<Type>
    [[nodiscard]] constexpr Type BranchlessIf(const bool condition, const Type lhs, const Type rhs) noexcept;

    /** @brief Branchless compute if */
    template<typename Type>
        requires std::is_enum_v<Type>
    [[nodiscard]] constexpr Type BranchlessIf(const bool condition, const Type lhs, const Type rhs) noexcept
        { return Type(BranchlessIf(condition, Core::ToUnderlying(lhs), Core::ToUnderlying(rhs))); }


    /** @brief Linear interpolate function */
    template<typename Type, typename RatioType>
    [[nodiscard]] constexpr Type Lerp(const Type min, const Type max, const RatioType ratio) noexcept
        { return Type(RatioType(min) + RatioType(max - min) * ratio); }

    /** @brief Linear inversed interpolate function */
    template<typename RatioType, typename Type>
        requires (!std::is_floating_point_v<Type>)
    [[nodiscard]] constexpr RatioType InverseLerp(const Type min, const Type max, const Type value) noexcept
        { return RatioType(value - min) / RatioType(max - min); }

    /** @brief Linear inversed interpolate function */
    template<typename Type>
        requires std::is_floating_point_v<Type>
    [[nodiscard]] constexpr Type InverseLerp(const Type min, const Type max, const Type value) noexcept
        { return (value - min) / (max - min); }
}

#include "Utils.ipp"