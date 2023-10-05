/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Compile-time function decomposer
 */

#pragma once

#include <tuple>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, typename = void>
    struct FunctionDecomposer;

    /** @brief Describes a function */
    template<typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Args...)>
    {
        using ClassType = void;
        using ReturnType = Return;
        using ArgsTuple = std::tuple<Args...>;

        static constexpr std::index_sequence_for<Args...> IndexSequence {};
        static constexpr bool IsConst = false;
        static constexpr bool IsMember = false;
        static constexpr bool IsFunctor = false;
        static constexpr bool IsNoexcept = false;
    };

    /** @brief Describes a noexcept function */
    template<typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Args...) noexcept> : FunctionDecomposer<Return(Args...)>
    {
        static constexpr bool IsNoexcept = true;
    };

    /** @brief Describes a member function */
    template<typename Class, typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Class::*)(Args...)> : FunctionDecomposer<Return(Args...)>
    {
        using ClassType = Class;

        static constexpr bool IsMember = true;
    };

    /** @brief Describes a noexcept member function */
    template<typename Class, typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Class::*)(Args...) noexcept> : FunctionDecomposer<Return(Class::*)(Args...)>
    {
        static constexpr bool IsNoexcept = true;
    };

    /** @brief Describes a const member function */
    template<typename Class, typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Class::*)(Args...) const> : FunctionDecomposer<Return(Class::*)(Args...)>
    {
        static constexpr bool IsConst = true;
    };

    /** @brief Describes a const noexcept member function */
    template<typename Class, typename Return, typename ...Args>
    struct FunctionDecomposer<Return(Class::*)(Args...) const noexcept> : FunctionDecomposer<Return(Class::*)(Args...) const>
    {
        static constexpr bool IsNoexcept = true;
    };

    /** @brief Describes an std::function */
    template<typename Type>
    struct FunctionDecomposer<Type, decltype(void(&Type::operator()))> : FunctionDecomposer<decltype(&Type::operator())>
    {
        static constexpr bool IsFunctor = true;
    };

    /** @brief Helper that catch functions and get their Decomposer */
    template<typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Args...)> ToFunctionDecomposer(Return(Args...));

    /** @brief Helper that catch noexcept functions and get their Decomposer */
    template<typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Args...) noexcept> ToFunctionDecomposer(Return(Args...) noexcept);

    /** @brief Helper that catch member functions and get their Decomposer */
    template<typename Class, typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Class::*)(Args...)> ToFunctionDecomposer(Return(Class::*)(Args...));

    /** @brief Helper that catch noexcept member functions and get their Decomposer */
    template<typename Class, typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Class::*)(Args...) noexcept> ToFunctionDecomposer(Return(Class::*)(Args...) noexcept);

    /** @brief Helper that catch const member functions and get their Decomposer */
    template<typename Class, typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Class::*)(Args...) const> ToFunctionDecomposer(Return(Class::*)(Args...) const);

    /** @brief Helper that catch const noexcept member functions and get their Decomposer */
    template<typename Class, typename Return, typename ...Args>
    constexpr FunctionDecomposer<Return(Class::*)(Args...) const noexcept> ToFunctionDecomposer(Return(Class::*)(Args...) const noexcept);

    /** @brief Helper that catch functors and get their Decomposer */
    template<typename Type>
    constexpr FunctionDecomposer<Type, decltype(void(&Type::operator()))> ToFunctionDecomposer(const Type &);

    constexpr void ToFunctionDecomposer(...);

    /** @brief Helper that retreive Decomposer of any function signature */
    template<typename FunctionType>
    using FunctionDecomposerHelper = decltype(ToFunctionDecomposer(std::declval<FunctionType>()));
}
