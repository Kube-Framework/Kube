/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tuple utilities
 */

#pragma once

#include <tuple>
#include <type_traits>

namespace kF::Core
{
    namespace Internal
    {
        /** @brief Helper used to recursivly search for index of a type */
        template<typename Search, typename Tuple>
        struct SearchTupleElementIndexHelper;

        /** @brief Helper reached end of tuple */
        template<typename Search>
        struct SearchTupleElementIndexHelper<Search, std::tuple<>>
        {
            static constexpr std::size_t Value = 0;
        };

        /** @brief Helper found index and has to ensure 'Search' appears only once */
        template<typename Search, typename ...Nexts>
        struct SearchTupleElementIndexHelper<Search, std::tuple<Search, Nexts...>>
        {
            static constexpr std::size_t Value = 0;

            using NextsTuple = std::tuple<Nexts...>;

            static_assert(SearchTupleElementIndexHelper<Search, NextsTuple>::Value == std::tuple_size_v<NextsTuple>,
                "TupleElementIndex: 'Search' type appears more than once in 'Tuple'");
        };

        /** @brief Helper did not found 'Search' type and continue searching */
        template<typename Search, typename First, typename ...Nexts>
        struct SearchTupleElementIndexHelper<Search, std::tuple<First, Nexts...>>
        {
            using RestTuple = std::tuple<Nexts...>;

            static constexpr std::size_t Value = 1 + SearchTupleElementIndexHelper<Search, RestTuple>::Value;
        };

        /** @brief Start helper recursion and ensure the index is in tuple range */
        template<typename Search, typename Tuple>
        struct SearchTupleElementIndex
        {
            static constexpr std::size_t Value = SearchTupleElementIndexHelper<Search, Tuple>::Value;

            static_assert(Value < std::tuple_size_v<Tuple>, "TupleElementIndex: 'Search' type does not appear in 'Tuple'");
        };
    }

    /** @brief Get the index of a type inside a tuple */
    template<typename Search, typename Tuple, typename Range = std::size_t>
    constexpr Range TupleElementIndex = static_cast<Range>(Internal::SearchTupleElementIndex<Search, Tuple>::Value);

    /** @brief Check if a type match a tuple element */
    template<typename Search, typename Tuple>
    constexpr bool TupleContainsElement = []<typename ...Types>(std::type_identity<std::tuple<Types...>>) {
        return (std::is_same_v<Search, Types> || ...);
    }(std::type_identity<Tuple> {});

    namespace Internal
    {
        /** @brief Helper to remove an element from a tuple */
        template<std::size_t Index, typename Tuple>
        struct RemoveTupleElementHelper {};

        /** @brief Helper to remove an element from a tuple */
        template<typename Current, typename ...Nexts>
        struct RemoveTupleElementHelper<0, std::tuple<Current, Nexts...>>
        {
            using Type = std::tuple<Nexts...>;
        };

        /** @brief Helper to remove an element from a tuple */
        template<std::size_t Index, typename Current, typename ...Nexts>
        struct RemoveTupleElementHelper<Index, std::tuple<Current, Nexts...>>
        {
            using Type = decltype(
                std::tuple_cat(
                    std::declval<std::tuple<Current>>(),
                    std::declval<typename RemoveTupleElementHelper<Index - 1, std::tuple<Nexts...>>::Type>()
                )
            );
        };
    }

    /** @brief Remove the ith element from a tuple type */
    template<std::size_t Index, typename Tuple>
    using RemoveTupleElement = Internal::RemoveTupleElementHelper<Index, Tuple>::Type;

    /** @brief Concatenate tuple types */
    template<typename Lhs, typename Rhs>
    using ConcatenateTuple = decltype(std::tuple_cat<Lhs, Rhs>(std::declval<Lhs>(), std::declval<Rhs>()));


    namespace Internal
    {
        /** @brief Helper to check if a function can be used with 'std::apply' */
        template<typename, typename>
        struct IsApplicableHelper : std::false_type {};

        /** @brief Helper to check if a function can be used with 'std::apply' */
        template<typename Func, template<typename...> class Tuple, typename... Args>
        struct IsApplicableHelper<Func, Tuple<Args...>> : std::is_invocable<Func, Args...> {};

        /** @brief Helper to check if a function can be used with 'std::apply' */
        template<typename Func, template<typename...> class Tuple, typename... Args>
        struct IsApplicableHelper<Func, const Tuple<Args...>> : std::is_invocable<Func, Args...> {};
    }

    /** @brief Same as std::is_invocable_v but for std::apply */
    template<typename Func, typename Args>
    constexpr bool IsApplicable = Internal::IsApplicableHelper<Func, Args>::value;
}