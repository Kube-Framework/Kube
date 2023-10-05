/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: System
 */

#include <Kube/Core/Abort.hpp>
#include <Kube/Core/FunctionDecomposer.hpp>

#include "System.hpp"


template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::pack(void) noexcept
{
    constexpr auto Pack = []<typename Table>(Table &table) {
        static_assert(Table::IsStable == true, "ECS::System::pack: Components must be declared Stable to enable manual pack, else they are always packed");
        table.pack();
    };

    (Pack(getTable<Components>()), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
bool kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::exists(const ECS::Entity entity) const noexcept
{
    return (getTable<Components>().exists(entity) && ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
        requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline kF::ECS::Entity kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::add(Components &&...components) noexcept
{
    const auto entity = add();

    attach(entity, std::forward<Components>(components)...);
    return entity;
}


template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline kF::ECS::EntityRange kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::addRange(const Entity count, Components &&...components) noexcept
{
    const auto range = addRange(count);

    attachRange(range, std::forward<Components>(components)...);
    return range;
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::attach(const Entity entity, Components &&...components) noexcept
{
    ((getTable<Components>().add(entity, std::forward<Components>(components))), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::tryAttach(const Entity entity, Components &&...components) noexcept
{
    ((getTable<Components>().tryAdd(entity, std::forward<Components>(components))), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Functors>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::tryAttach(const Entity entity, Functors &&...functors) noexcept
{
    const auto apply = [this, entity]<typename Functor>(Functor &&functor) {
        using Decomposer = Core::FunctionDecomposerHelper<Functor>;
        using Component = std::remove_cvref_t<std::tuple_element_t<0, typename Decomposer::ArgsTuple>>;
        getTable<Component>().tryAdd(entity, std::forward<Functor>(functor));
    };

    ((apply(std::forward<Functors>(functors))), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::attachRange(const EntityRange range, Components &&...components) noexcept
{
    ((getTable<Components>().addRange(range, std::forward<Components>(components))), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::dettach(const Entity entity) noexcept
{
    ((getTable<Components>().remove(entity)), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::tryDettach(const Entity entity) noexcept
{
    ((getTable<Components>().tryRemove(entity)), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::dettachRange(const EntityRange range) noexcept
{
    ((getTable<Components>().removeRange(range)), ...);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::remove(const Entity entity) noexcept
{
    const auto wrapper = [this]<Entity ...Indexes>([[maybe_unused]] const Entity entity, const std::integer_sequence<Entity, Indexes...>) {
        ((std::get<Indexes>(_tables).tryRemove(entity)), ...);
    };

    wrapper(entity, std::make_integer_sequence<Entity, ComponentCount> {});
    Internal::ASystem::remove(entity);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::removeRange(const EntityRange range) noexcept
{
    const auto wrapper = [this]<Entity ...Indexes>(const EntityRange range, const std::integer_sequence<Entity, Indexes...>) {
        ((std::get<Indexes>(_tables).removeRange(range)), ...);
    };

    wrapper(range, std::make_integer_sequence<Entity, ComponentCount> {});
    Internal::ASystem::removeRange(range);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::removeUnsafe(const Entity entity) noexcept
{
    ((getTable<Components>().remove(entity)), ...);
    Internal::ASystem::remove(entity);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename ...Components>
    requires kF::ECS::SystemComponentRequirements<kF::ECS::Internal::ForwardComponentsTuple<ComponentTypes...>, Components...>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::removeUnsafeRange(const EntityRange range) noexcept
{
    ((getTable<Components>().removeRange(range)), ...);
    Internal::ASystem::removeRange(range);
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<bool RetryOnFailure, typename Callback>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::interact(Callback &&callback) const noexcept
{
    using Decomposer = Core::FunctionDecomposerHelper<Callback>;
    using DestinationSystem = std::tuple_element_t<0, typename Decomposer::ArgsTuple>;
    using FlatSystem = std::remove_reference_t<DestinationSystem>;

    interact<typename FlatSystem::ExecutorPipeline, RetryOnFailure>(std::forward<Callback>(callback));
}

template<kF::Core::FixedString Literal, kF::ECS::Pipeline TargetPipeline, kF::Core::StaticAllocatorRequirements Allocator, typename ...ComponentTypes>
template<typename DestinationPipeline, bool RetryOnFailure, typename Callback>
inline void kF::ECS::System<Literal, TargetPipeline, Allocator, ComponentTypes...>::interact(Callback &&callback) const noexcept
{
    const auto invoke = [this](auto &callback) {
        using Decomposer = Core::FunctionDecomposerHelper<Callback>;

        // Callback without argument
        if constexpr (Decomposer::IndexSequence.size() == 0)
            callback();
        // Callback with argument
        else {
            using DestinationSystem = std::tuple_element_t<0, typename Decomposer::ArgsTuple>;
            using FlatSystem = std::remove_reference_t<DestinationSystem>;
            using ExecutorPipeline = typename FlatSystem::ExecutorPipeline;

            static_assert(Decomposer::IndexSequence.size() == 1 && std::is_base_of_v<Internal::ASystem, FlatSystem>
                && std::is_reference_v<DestinationSystem>,
                "ECS::System::interact: Event callback must only have one argument that must be a reference to any system");

            static_assert(std::is_same_v<DestinationPipeline, ExecutorPipeline>,
                "ECS::System::interact: Mismatching destination pipeline and destination system's pipeline");

            // Get system at runtime and call now
            const auto pipelineIndex = getPipelineIndex(DestinationPipeline::Hash);
            kFEnsure(pipelineIndex.success(),
                "ECS::System::interact: Pipeline '", DestinationPipeline::Name, "' is not registered");
            const auto system = getSystemOpaque(*pipelineIndex, FlatSystem::Hash);
            kFEnsure(system,
                "ECS::System::interact: System '", FlatSystem::Name, "' is not registered (Pipeline: '", ExecutorPipeline::Name, "')");
            callback(*reinterpret_cast<FlatSystem *>(system));
        }
    };

    // If 'this' pipeline is same as destination pipeline, invoke now
    if constexpr (std::is_same_v<ExecutorPipeline, DestinationPipeline>) {
        invoke(std::forward<Callback>(callback));
    // Else, send event to target pipeline
    } else {
        const auto pipelineIndex = getPipelineIndex(DestinationPipeline::Hash);
        kFEnsure(pipelineIndex.success(),
            "ECS::System::interact: Pipeline '", DestinationPipeline::Name, "' is not registered");
        sendEventOpaque<RetryOnFailure>(*pipelineIndex, [invoke, callback = std::forward<Callback>(callback)]() mutable {
            // 'pipelineIndex' could be cached but this would increase chances to use allocation
            invoke(callback);
        });
    }
}
