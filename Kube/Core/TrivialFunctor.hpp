/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial opaque functor holder
 */

#pragma once

#include <array>

#include "FunctorUtils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t DesiredSize = CacheLineQuarterSize>
    class TrivialFunctor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of TrivialFunctor and fit into its cache */
        template<typename Functor, std::size_t CacheSize>
        concept TrivialFunctorCacheRequirements = std::is_trivially_copyable_v<Functor> && sizeof(Functor) <= CacheSize;
    };
}

/** @brief Fast opaque functor but only takes trivial types that fit 'CacheSize' */
template<typename Return, typename ...Args, std::size_t DesiredSize>
class kF::Core::TrivialFunctor<Return(Args...), DesiredSize>
{
public:
    static_assert(
        DesiredSize >= Core::CacheLineQuarterSize && !(Core::CacheLineEighthSize % 8),
        "TrivialFunctor's DesiredSize must be at least 16 bytes sized and 8 byte aligned"
    );

    /** @brief Size of the optimized cache */
    static constexpr auto CacheSize = DesiredSize - Core::CacheLineEighthSize;

    /** @brief Byte cache */
    using Cache = std::array<std::byte, CacheSize>;

    /** @brief Trivial functor signature */
    using OpaqueInvoke = Return(*)(Cache &cache, Args...args);


    /** @brief Cast a cache into a given type */
    template<typename As>
    [[nodiscard]] static inline As &CacheAs(Cache &cache) noexcept
        { return reinterpret_cast<As &>(cache); }


    /** @brief Construct a member function */
    template<auto MemberFunction, typename ClassType>
        requires MemberInvocableRequirements<MemberFunction, ClassType, Return, Args...>
    [[nodiscard]] static inline TrivialFunctor Make(ClassType &&instance) noexcept
        { TrivialFunctor func; func.prepare<MemberFunction>(std::forward<ClassType>(instance)); return func; }

    /** @brief Construct a free function */
    template<auto Function>
        requires InvocableRequirements<decltype(Function), Return, Args...>
    [[nodiscard]] static inline TrivialFunctor Make(void) noexcept
        { TrivialFunctor func; func.prepare<Function>(); return func; }


    /** @brief Destructor */
    inline ~TrivialFunctor(void) noexcept = default;

    /** @brief Default constructor */
    inline TrivialFunctor(void) noexcept = default;

    /** @brief Copy constructor */
    inline TrivialFunctor(const TrivialFunctor &other) noexcept = default;

    /** @brief Move constructor */
    inline TrivialFunctor(TrivialFunctor &&other) noexcept = default;

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<TrivialFunctor, std::remove_cvref_t<ClassFunctor>>
            && Internal::TrivialFunctorCacheRequirements<ClassFunctor, CacheSize>)
    inline TrivialFunctor(ClassFunctor &&functor) noexcept { prepare(std::forward<ClassFunctor>(functor)); }


    /** @brief Copy assignment*/
    inline TrivialFunctor &operator=(const TrivialFunctor &other) noexcept = default;

    /** @brief Move assignment*/
    inline TrivialFunctor &operator=(TrivialFunctor &&other) noexcept = default;


    /** @brief Check if the functor is prepared */
    [[nodiscard]] inline operator bool(void) const noexcept { return _invoke; }


    /** @brief Release cache (nothing to free) */
    inline void release(void) noexcept { _invoke = nullptr; }


    /** @brief Prepare a functor */
    template<typename ClassFunctor>
        requires (!std::is_same_v<TrivialFunctor, std::remove_cvref_t<ClassFunctor>>
            && Internal::TrivialFunctorCacheRequirements<ClassFunctor, CacheSize>
            && InvocableRequirements<ClassFunctor, Return, Args...>)
    inline void prepare(ClassFunctor &&functor) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) noexcept -> Return {
            if constexpr (std::is_same_v<Return, void>)
                Invoke(CacheAs<ClassFunctor>(cache), std::forward<Args>(args)...);
            else
                return Invoke(CacheAs<ClassFunctor>(cache), std::forward<Args>(args)...);
        };
        new (&_cache) ClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a member function */
    template<auto MemberFunction, typename ClassType>
        requires MemberInvocableRequirements<MemberFunction, ClassType, Return, Args...>
    inline void prepare(ClassType &&instance) noexcept
    {
        using MemberClass = std::remove_reference_t<std::remove_pointer_t<ClassType>>;

        _invoke = [](Cache &cache, Args ...args) noexcept -> Return {
            return Invoke(MemberFunction, CacheAs<MemberClass *>(cache), std::forward<Args>(args)...);
        };
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
        _invoke = [](Cache &, Args ...args) noexcept -> Return {
            return Invoke(Function, std::forward<Args>(args)...);
        };
    }

    /** @brief Invoke internal functor */
    inline Return operator()(Args ...args) const noexcept { return _invoke(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueInvoke _invoke {};
    Cache _cache {};
};