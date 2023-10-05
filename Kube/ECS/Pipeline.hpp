/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#pragma once

#include <Kube/Core/Hash.hpp>
#include <Kube/Core/FixedString.hpp>

namespace kF::ECS
{
    /** @brief Different time managing modes */
    enum class PipelineTimeMode
    {
        Free,
        Bound
    };

    /** @brief Pipeline unique type tag
     *  @param Literal Pipeline's unique name */
    template<Core::FixedString Literal>
    struct PipelineTag
    {
        /** @brief Pipeline name */
        static constexpr auto Name = Literal.toView();

        /** @brief Pipeline hashed name */
        static constexpr auto Hash = Core::Hash(Name);
    };

    /** @brief Pipeline concept */
    template<typename Type>
    concept Pipeline = Core::IsTag<Type, PipelineTag>;
}
