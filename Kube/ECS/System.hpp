/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: System
 */

#pragma once

#include <Kube/Core/TupleUtils.hpp>

#include "ASystem.hpp"
#include "Pipeline.hpp"
#include "ComponentTable.hpp"
#include "StableComponentTable.hpp"

namespace kF::ECS
{
    class Executor;

    /** @brief Component stable tag (use StableComponentTable) */
    template<typename ComponentType, EntityIndex ComponentPageSize = Core::NextPowerOf2(4096 / sizeof(ComponentType))>
    struct StableComponent
    {
        /** @brief Underyling type */
        using ValueType = ComponentType;

        /** @brief Underyling page size */
        static constexpr auto PageSize = ComponentPageSize;
    };

    namespace Internal
    {
        /** @brief Forward component base */
        template<typename ComponentType>
        struct ForwardComponent;

        /** @brief Forward component common case */
        template<typename ComponentType>
        struct ForwardComponent
        {
            using Type = ComponentType;
        };

        /** @brief Forward component stable tag*/
        template<typename ComponentType, EntityIndex ComponentPageSize>
        struct ForwardComponent<StableComponent<ComponentType, ComponentPageSize>> : ForwardComponent<ComponentType> {};


        /** @brief Forward table base */
        template<typename ComponentType, EntityIndex EntityPageSize, typename Allocator>
        struct ForwardComponentTable;

        /** @brief Forward table common case */
        template<typename ComponentType, EntityIndex EntityPageSize, typename Allocator>
        struct ForwardComponentTable
        {
            using Type = ComponentTable<ComponentType, EntityPageSize, Allocator>;
        };

        /** @brief Forward table stable tag*/
        template<typename ComponentType, EntityIndex ComponentPageSize, EntityIndex EntityPageSize, typename Allocator>
        struct ForwardComponentTable<StableComponent<ComponentType, ComponentPageSize>, EntityPageSize, Allocator>
        {
            using Type = StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>;
        };


        /** @brief Tuple of forwarded components */
        template<typename ...ComponentTypes>
        using ForwardComponentsTuple = std::tuple<typename ForwardComponent<ComponentTypes>::Type...>;
    }

    template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
    class System;


    /** @brief Concept that ensure multiple Components match a ComponentsTuple */
    template<typename ComponentsTuple, typename ...Components>
    concept SystemComponentRequirements = (Core::TupleContainsElement<std::remove_cvref_t<Components>, ComponentsTuple> && ...);
}

/** @brief Abstract class of any system that contains meta data about the system
 *  @tparam Literal Name of the System
 *  @tparam Pipeline System's execution pipeline */
template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator =
        kF::Core::DefaultStaticAllocator, typename ...ComponentTypes>
class alignas(sizeof...(ComponentTypes) ? kF::Core::CacheLineDoubleSize : kF::Core::CacheLineSize)
        kF::ECS::System : public Internal::ASystem
{
public:
    /** @brief Name of the system */
    static constexpr auto Name = Literal.toView();

    /** @brief Hashed name of the system */
    static constexpr auto Hash = Core::Hash(Name);

    /** @brief Entity page size */
    static constexpr Entity EntityPageSize = 4096 / sizeof(kF::ECS::Entity);

    /** @brief Execution pipeline of the system */
    using ExecutorPipeline = TargetPipeline;


    /** @brief Static component list */
    using ComponentsTuple = Internal::ForwardComponentsTuple<ComponentTypes...>;

    /** @brief Static component table list */
    using ComponentTablesTuple = std::tuple<typename Internal::ForwardComponentTable<ComponentTypes, EntityPageSize, Allocator>::Type...>;

    /** @brief Number of component tables in this system */
    static constexpr std::size_t ComponentCount = sizeof...(ComponentTypes);


    /** @brief Virtual destructor */
    virtual ~System(void) noexcept override = default;

    /** @brief Default constructor */
    System(void) noexcept { queryPipelineIndex(ExecutorPipeline::Hash); }


    /** @brief Virtual pipeline name getter */
    [[nodiscard]] constexpr std::string_view pipelineName(void) const noexcept final { return ExecutorPipeline::Name; }

    /** @brief Virtual system name getter */
    [[nodiscard]] constexpr std::string_view systemName(void) const noexcept final { return Name; }


    /** @brief Interact with another system using 'this'
     *  @note The callback functor must have a system reference as argument : void(auto &system)
     *  @note If 'this' system and target system are not on the same pipeline, an event is sent to the target pipeline */
    template<bool RetryOnFailure = true, typename Callback>
    void interact(Callback &&callback) const noexcept;

    /** @brief Interact with another system using 'this'
     *  @note The callback functor may have a system reference as argument : void(void) | void(auto &system)
     *  @note If 'this' system pipeline is not the same as target pipeline, an event is sent to the target pipeline
     *  @tparam DestinationPipeline Pipeline to which belong the callback */
    template<typename DestinationPipeline, bool RetryOnFailure = true, typename Callback>
    void interact(Callback &&callback) const noexcept;


    /** @brief Pack stable component tables */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void pack(void) noexcept;


    /** @brief Check an entity has the given Components */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    [[nodiscard]] bool exists(const ECS::Entity entity) const noexcept;


    /** @brief Creates an entity */
    using Internal::ASystem::add;

    /** @brief Creates an entity with components */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    Entity add(Components &&...components) noexcept;

    /** @brief Creates a range of entities */
    using Internal::ASystem::addRange;

    /** @brief Creates a range of entities with components */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    EntityRange addRange(const Entity count, Components &&...components) noexcept;


    /** @brief Attach components to an entity */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void attach(const Entity entity, Components &&...components) noexcept;

    /** @brief Attach components to an entity, if the component already exists then update it */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void tryAttach(const Entity entity, Components &&...components) noexcept;

    /** @brief Try to update components of an entity
     *  @note If a component doesn't exists, it is created */
    template<typename ...Functors>
    void tryAttach(const Entity entity, Functors &&...functors) noexcept;

    /** @brief Attach components to a range of entities */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void attachRange(const EntityRange range, Components &&...components) noexcept;


    /** @brief Dettach components from an entity */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void dettach(const Entity entity) noexcept;

    /** @brief Dettach components from an entity */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void tryDettach(const Entity entity) noexcept;

    /** @brief Dettach components from a range of entities */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void dettachRange(const EntityRange range) noexcept;


    /** @brief Removes an entity */
    void remove(const Entity entity) noexcept;

    /** @brief Removes a range of entities */
    void removeRange(const EntityRange range) noexcept;

    /** @brief Removes an entity, knowing attached components at compile time
     *  @note Any component attached that is not referenced inside 'Components' will not get destroyed */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void removeUnsafe(const Entity entity) noexcept;

    /** @brief Removes an entity, without destroying its components */
    inline void removeUnsafe(const Entity entity) noexcept { Internal::ASystem::remove(entity); }

    /** @brief Removes a range of entities, knowing attached components at compile time
     *  @note Any component attached that is not referenced inside 'Components' will not get destroyed */
    template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
    void removeUnsafeRange(const EntityRange range) noexcept;

    /** @brief Removes a range of entities, without destroying its components */
    inline void removeUnsafeRange(const EntityRange range) noexcept { Internal::ASystem::removeRange(range); }


    /** @brief Get a component table by component type */
    template<typename Component>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Component>
    [[nodiscard]] inline auto &getTable(void) noexcept
        { return std::get<Core::TupleElementIndex<std::remove_cvref_t<Component>, ComponentsTuple>>(_tables); }
    template<typename Component>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Component>
    [[nodiscard]] inline const auto &getTable(void) const noexcept
        { return std::get<Core::TupleElementIndex<std::remove_cvref_t<Component>, ComponentsTuple>>(_tables); }


    /** @brief Get a component using its type and an entity */
    template<typename Component>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Component>
    [[nodiscard]] inline Component &get(const Entity entity) noexcept
        { return getTable<Component>().get(entity); }
    template<typename Component>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Component>
    [[nodiscard]] inline const Component &get(const Entity entity) const noexcept
        { return getTable<Component>().get(entity); }


    /** @brief Calls a functor for each component table */
    template<typename Functor>
    inline void forEachTable(Functor &&delegate) noexcept
        { (delegate(get<ComponentTypes>()), ...); }

private:
    using Internal::ASystem::queryPipelineIndex;
    using Internal::ASystem::remove;
    using Internal::ASystem::removeRange;

    // Cacheline 1 -> ?
    [[no_unique_address]] ComponentTablesTuple _tables {};
};

#include "System.ipp"
