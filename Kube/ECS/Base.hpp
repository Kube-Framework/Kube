/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ECS Base
 */

#pragma once

#include <Kube/Core/StaticSafeAllocator.hpp>

namespace kF::ECS
{
    /** @brief Allocator of the ECS library */
    struct ECSAllocator : Core::StaticSafeAllocator<"ECSAllocator"> {};


    /** @brief Entity */
    using Entity = std::uint32_t;

    /** @brief Entity index */
    using EntityIndex = Entity;

    /** @brief Special null entity */
    static constexpr Entity NullEntity = ~static_cast<Entity>(0);

    /** @brief Special null index */
    static constexpr EntityIndex NullEntityIndex = ~static_cast<EntityIndex>(0);


    /** @brief Entity index range */
    struct alignas_eighth_cacheline EntityRange
    {
        Entity begin {};
        Entity end {};

        /** @brief Comparison operators */
        [[nodiscard]] constexpr bool operator==(const EntityRange &other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const EntityRange &other) const noexcept = default;

        /** @brief Check if an entity is inside the range */
        [[nodiscard]] constexpr bool contains(const ECS::Entity entity) const noexcept { return (entity >= begin) & (entity < end); }

        /** @brief Get entity range size */
        [[nodiscard]] constexpr EntityIndex size(void) const noexcept { return end - begin; }
    };

    /** @brief Number of bits in entity type */
    constexpr Entity EntityBitCount = sizeof(Entity) * 8;


    /** @brief Convert hertz into time rate */
    [[nodiscard]] constexpr std::int64_t HzToRate(const std::int64_t hertz) noexcept
        { return 1'000'000'000ll / hertz; }

    namespace Internal
    {
        /** @brief Initializer of entity indexes */
        constexpr void EntityIndexInitializer(EntityIndex * const begin, EntityIndex * const end) noexcept
            { std::fill(begin, end, NullEntityIndex); }
    }
}