/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FunctorUtils
 */

#pragma once

#include <cstddef>
#include <functional>

#include "FunctionDecomposer.hpp"

namespace kF::Core
{
    /** @brief Types of functor implementations */
    enum class FunctorType
    {
        Error,
        Trivial,
        Allocated
    };

    /** @brief Functor type detector, Error case */
    template<typename ObjectType>
    struct FunctorTypeDetector
    {
        static constexpr FunctorType Type = FunctorType::Error;
    };

    /** @brief Functor type detector, Trivial case */
    template<template<typename, std::size_t> typename ObjectType, typename Arg, std::size_t CacheSize>
    struct FunctorTypeDetector<ObjectType<Arg, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Trivial;
    };

    /** @brief Functor type detector, Allocated case */
    template<template<typename, typename, std::size_t> typename ObjectType, typename Arg, typename Allocator, std::size_t CacheSize>
    struct FunctorTypeDetector<ObjectType<Arg, Allocator, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Allocated;
    };

    /** @brief Concept of a dispatcher functor */
    template<typename ObjectType>
    concept FunctorRequirements = FunctorTypeDetector<ObjectType>::Type != FunctorType::Error;


    namespace Internal
    {
        /** @brief Invoke a function with a variable list of arguments */
        template<typename FunctionArgs, typename Function, typename ArgsTuple, std::size_t ...Sequence>
            requires std::is_invocable_v<Function, std::tuple_element_t<Sequence, ArgsTuple>...>
        constexpr decltype(auto) InvokeImpl(std::index_sequence<Sequence...>, Function &&function, ArgsTuple &&args) noexcept
        {
            return std::invoke(std::forward<Function>(function), std::forward<std::tuple_element_t<Sequence, ArgsTuple>>(std::get<Sequence>(args))...);
        }

        /** @brief Invoke a function with a variable list of arguments - error case */
        template<typename FunctionArgs, typename Function, typename ArgsTuple, std::size_t ...Sequence>
            requires (!std::is_invocable_v<Function, std::tuple_element_t<Sequence, ArgsTuple>...> && sizeof...(Sequence) == 0)
        constexpr decltype(auto) InvokeImpl(std::index_sequence<Sequence...>, Function &&, ArgsTuple &&) noexcept
        {
            static_assert(
                std::is_same<FunctionArgs, ArgsTuple>::value,
                "Core::Invoke: Arguments passed can't match functor signature"
            );
        }

        /** @brief Remove arguments one by one to find a matching combination */
        template<typename FunctionArgs, typename Function, typename ArgsTuple, std::size_t ...Sequence>
            requires (!std::is_invocable_v<Function, std::tuple_element_t<Sequence, ArgsTuple>...> && sizeof...(Sequence) > 0)
        constexpr decltype(auto) InvokeImpl(std::index_sequence<Sequence...>, Function &&function, ArgsTuple &&args) noexcept
        {
            return Internal::InvokeImpl<FunctionArgs>(
                std::make_index_sequence<sizeof...(Sequence) - 1>(),
                std::forward<Function>(function),
                std::move(args)
            );
        }
    }

    /** @brief Invoke a function with a variable list of arguments
     *  @note The list of runtime arguments may be larger than function's argument list, they are ignored */
    template<typename Function, typename ...Args>
    constexpr decltype(auto) Invoke(Function &&function, Args &&...args) noexcept
    {
        return Internal::InvokeImpl<typename FunctionDecomposerHelper<Function>::ArgsTuple>(
            std::make_index_sequence<sizeof...(Args)>(),
            std::forward<Function>(function),
            std::forward_as_tuple(std::forward<Args>(args)...)
        );
    }

    /** @brief Check if a given functor / function is callable */
    template<typename Functor, typename Return, typename ...Args>
    constexpr bool IsInvocable = requires(Functor &functor, Args ...args) {
        { Invoke(functor, std::forward<Args>(args)...) } -> std::convertible_to<Return>;
    };

    /** @brief Ensure that a given functor / function is callable */
    template<typename Functor, typename Return, typename ...Args>
    concept InvocableRequirements = IsInvocable<Functor, Return, Args...>;

    /** @brief Ensure that a given member function is callable */
    template<auto Member, typename ClassType, typename Return, typename ...Args>
    concept MemberInvocableRequirements =
            std::is_invocable_r_v<Return, decltype(Invoke<decltype(Member), ClassType, Args...>), decltype(Member), ClassType, Args...>;
}