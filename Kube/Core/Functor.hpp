/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Functor
 */

#pragma once

#include <cstring>

#include "FunctorUtils.hpp"

namespace kF::Core
{
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
            std::size_t DesiredSize = CacheLineHalfSize>
    class Functor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorCacheRequirements = std::is_trivially_copyable_v<Functor> && sizeof(Functor) <= CacheSize;

        /** @brief Ensure that a given functor DOES NOT met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorNoCacheRequirements = (std::is_trivially_copyable_v<Functor> ? sizeof(Functor) > CacheSize : true);
    }
}

/** @brief General opaque functor that can optimize trivial types that fit 'DesiredSize' */
template<typename Return, typename ...Args, kF::Core::StaticAllocatorRequirements Allocator, std::size_t DesiredSize>
class kF::Core::Functor<Return(Args...), Allocator, DesiredSize>
{
public:
    static_assert(
        DesiredSize >= Core::CacheLineEighthSize * 3 && !(Core::CacheLineEighthSize % 8),
        "Functor's DesiredSize must be at least 24 bytes sized and 8 byte aligned"
    );

    /** @brief Size of the optimized cache */
    static constexpr auto CacheSize = DesiredSize - Core::CacheLineQuarterSize;

    /** @brief Byte cache */
    using Cache = std::byte[CacheSize];

    /** @brief Functor signature */
    using OpaqueInvoke = Return(*)(Cache &cache, Args...args);
    using OpaqueDestructor = void(*)(Cache &cache);

    /** @brief Functor return type */
    using ReturnType = Return;

    /** @brief Structure describing a runtime allocation inside the functor */
    struct alignas_eighth_cacheline RuntimeAllocation
    {
        void *ptr {};
    };
    static_assert_fit_eighth_cacheline(RuntimeAllocation);

    static_assert(CacheSize >= sizeof(RuntimeAllocation),
        "Functor's cache size must be at least the size of RuntimeAllocation (8)");

    /** @brief Cast a cache into a given type */
    template<typename As>
    [[nodiscard]] static inline As &CacheAs(Cache &cache) noexcept
        { return reinterpret_cast<As &>(cache); }


    /** @brief Construct a member function */
    template<auto MemberFunction, typename ClassType>
        requires MemberInvocableRequirements<MemberFunction, ClassType, Return, Args...>
    [[nodiscard]] static inline Functor Make(ClassType &&instance) noexcept
        { Functor func; func.prepare<MemberFunction>(std::forward<ClassType>(instance)); return func; }

    /** @brief Construct a free function */
    template<auto Function>
        requires InvocableRequirements<decltype(Function), Return, Args...>
    [[nodiscard]] static inline Functor Make(void) noexcept
        { Functor func; func.prepare<Function>(); return func; }


    /** @brief Destructor */
    inline ~Functor(void) noexcept { release<false>(); }

    /** @brief Default constructor */
    inline Functor(void) noexcept = default;

    /** @brief Move constructor */
    inline Functor(Functor &&other) noexcept
    {
        std::memcpy(this, &other, sizeof(Functor));
        other._invoke = nullptr;
        other._destruct = nullptr;
    }

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>)
    inline Functor(ClassFunctor &&functor) noexcept
        { prepare(std::forward<ClassFunctor>(functor)); }


    /** @brief Move assignment*/
    inline Functor &operator=(Functor &&other) noexcept
    {
        release<false>();
        std::memcpy(this, &other, sizeof(Functor));
        other._invoke = nullptr;
        other._destruct = nullptr;
        return *this;
    }

    /** @brief Destroy the functor and free its allocated memory
     *  Note that releasing without reseting members is not safe ! */
    template<bool ResetMembers = true>
    inline void release(void) noexcept
    {
        if (_destruct)
            _destruct(_cache);
        if constexpr (ResetMembers) {
            _invoke = nullptr;
            _destruct = nullptr;
        }
    }

    /** @brief Check if the functor is prepared */
    [[nodiscard]] inline operator bool(void) const noexcept { return _invoke; }


    /** @brief Prepare a trivial functor */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>
            && Internal::FunctorCacheRequirements<ClassFunctor, CacheSize>
            && InvocableRequirements<ClassFunctor, Return, Args...>)
    inline void prepare(ClassFunctor &&functor) noexcept
    {
        using FlatClassFunctor = std::remove_cvref_t<ClassFunctor>;

        release<false>();
        _invoke = [](Cache &cache, Args ...args) noexcept -> Return {
            if constexpr (std::is_same_v<Return, void>)
                Invoke(CacheAs<ClassFunctor>(cache), std::forward<Args>(args)...);
            else
                return Invoke(CacheAs<ClassFunctor>(cache), std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) FlatClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a non-trivial functor with an allocator */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>
            && Internal::FunctorNoCacheRequirements<ClassFunctor, CacheSize>
            && InvocableRequirements<ClassFunctor, Return, Args...>)
    inline void prepare(ClassFunctor &&functor) noexcept
    {
        using FlatClassFunctor = std::remove_cvref_t<ClassFunctor>;
        using ClassFunctorPtr = FlatClassFunctor *;

        auto &runtime = CacheAs<RuntimeAllocation>(_cache);

        release<false>();
        runtime.ptr = Allocator::Allocate(sizeof(FlatClassFunctor), alignof(FlatClassFunctor));
        new (runtime.ptr) FlatClassFunctor(std::forward<FlatClassFunctor>(functor));
        _invoke = [](Cache &cache, Args ...args) noexcept -> Return {
            return Invoke(*reinterpret_cast<ClassFunctorPtr &>(CacheAs<RuntimeAllocation>(cache).ptr), std::forward<Args>(args)...);
        };
        _destruct = [](Cache &cache) {
            auto &runtime = CacheAs<RuntimeAllocation>(cache);
            reinterpret_cast<ClassFunctorPtr &>(runtime.ptr)->~FlatClassFunctor();
            Allocator::Deallocate(runtime.ptr, sizeof(FlatClassFunctor), alignof(FlatClassFunctor));
        };
    }

    /** @brief Prepare a member function */
    template<auto MemberFunction, typename ClassType>
        requires MemberInvocableRequirements<MemberFunction, ClassType, Return, Args...>
    inline void prepare(ClassType &&instance) noexcept
    {
        using MemberClass = std::remove_reference_t<std::remove_pointer_t<ClassType>>;

        release<false>();
        _invoke = [](Cache &cache, Args ...args) noexcept -> Return {
            return Invoke(MemberFunction, CacheAs<MemberClass *>(cache), std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        if constexpr (std::is_pointer_v<ClassType>)
            new (&_cache) MemberClass *(instance);
        else
            new (&_cache) MemberClass *(&instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires InvocableRequirements<decltype(Function), Return, Args...>
    inline void prepare(void) noexcept
    {
        release<false>();
        _invoke = [](Cache &, Args ...args) noexcept -> Return {
            return Invoke(Function, std::forward<Args>(args)...);
        };
        _destruct = nullptr;
    }

    /** @brief Invoke internal functor */
    inline Return operator()(Args ...args) const noexcept { return _invoke(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueInvoke _invoke {};
    OpaqueDestructor _destruct {};
    Cache _cache {};
};