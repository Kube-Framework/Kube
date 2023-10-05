/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#include <Kube/Core/SmallVector.hpp>

#include "ComponentTable.hpp"

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline ComponentType &kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::add(const Entity entity, Args &&...args) noexcept
{
    kFAssert(!exists(entity),
        "ECS::ComponentTable::add: Entity '", entity, "' already exists");

    return addImpl(entity, std::forward<Args>(args)...);
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline ComponentType &kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::tryAdd(const Entity entity, ComponentType &&component) noexcept
{
    if (auto componentIndex = findIndex(entity); componentIndex != NullEntityIndex) [[likely]] {
        return get(entity) = std::move(component);
    } else {
        return addImpl(entity, std::move(component));
    }
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Functor>
inline ComponentType &kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::tryAdd(const Entity entity, Functor &&functor) noexcept
{
    ComponentType *ptr;
    if (const auto componentIndex = findIndex(entity); componentIndex != NullEntityIndex) [[likely]] {
        ptr = &atIndex(componentIndex);
    } else {
        ptr = &addImpl(entity);
    }
    functor(*ptr);
    return *ptr;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::addRange(const EntityRange range, const Args &...args) noexcept
{
    const auto lastIndex = _entities.size();
    const auto count = range.end - range.begin;

    if constexpr (KUBE_DEBUG_BUILD) {
        // Ensure no entity exists in range
        for (const auto entity : _entities) {
            kFEnsure(entity < range.begin || entity >= range.end,
                "ECS::ComponentTable::addRange: Entity '", entity, "' from entity range [", range.begin, ", ", range.end, "[ already exists");
        }
    }

    // Insert entities
    _entities.insertCustom(_entities.end(), count, [range](const auto count, const auto out) {
        for (Entity i = 0; i != count; ++i)
            out[i] = range.begin + i;
    });

    // Insert indexes
    for (Entity i = lastIndex, it = range.begin; it != range.end; ++i, ++it) {
        _indexSet.add(it, i);
    }

    // Insert components
    _components.insertCustom(_components.end(), count, [&args...](const auto count, const auto data) {
        for (EntityIndex index {}; index != count; ++index) {
            new (data + index) ComponentType(args...);
        }
    });
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline ComponentType &kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::addImpl(const Entity entity, Args &&...args) noexcept
{
    const auto componentIndex = _entities.size();
    _indexSet.add(entity, componentIndex);
    _entities.push(entity);
    return _components.push(std::forward<Args>(args)...);
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::remove(const Entity entity) noexcept
{
    kFAssert(exists(entity),
        "ECS::ComponentTable::remove: Entity '", entity, "' doesn't exists");
    removeImpl(entity, _indexSet.extract(entity));
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline bool kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::tryRemove(const Entity entity) noexcept
{
    if (const auto entityIndex = findIndex(entity); entityIndex != NullEntityIndex) [[likely]] {
        removeImpl(entity, entityIndex);
        return true;
    } else
        return false;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::removeRange(const EntityRange range) noexcept
{
    const auto removeBack = [this](const auto range, auto &last) {
        while (last) {
            const auto entity = _entities.at(last - 1u);
            if (!range.contains(entity))
                break;
            _indexSet.remove(entity);
            --last;
        }
    };

    auto last = _entities.size();
    removeBack(range, last);
    for (auto index = 0u; index != last; ++index) {
        Entity &target = _entities.at(index);
        if (!range.contains(target))
            continue;
        const auto toRemove = target;
        --last;
        target = _entities.at(last);
        atIndex(index) = std::move(atIndex(last));
        _indexSet.remove(toRemove);
        _indexSet.at(target) = index;
        removeBack(range, last);
    }
    _entities.erase(_entities.begin() + last, _entities.end());
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::removeImpl(const Entity entity, const EntityIndex entityIndex) noexcept
{
    if (_components.size() != entityIndex + 1) [[likely]] {
        const auto lastEntity = _entities.back();
        _indexSet.at(lastEntity) = entityIndex;
        _entities.at(entityIndex) = lastEntity;
        _components.at(entityIndex) = std::move(_components.back());
    }
    _indexSet.remove(entity);
    _entities.pop();
    _components.pop();
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline ComponentType kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::extract(const Entity entity) noexcept
{
    kFAssert(exists(entity),
        "ECS::ComponentTable::remove: Entity '", entity, "' doesn't exists");

    const auto componentIndex = _indexSet.extract(entity);
    ComponentType value(std::move(_components.at(componentIndex)));

    if (_components.size() != componentIndex + 1) [[likely]] {
        const auto lastEntity = _entities.back();
        _indexSet.at(lastEntity) = componentIndex;
        _entities.at(componentIndex) = lastEntity;
        _components.at(componentIndex) = std::move(_components.back());
    }
    _entities.pop();
    _components.pop();
    return value;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::ECS::EntityIndex kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::getUnstableIndex(const Entity entity) const noexcept
{
    if (_indexSet.pageExists(entity)) [[likely]]
        return _indexSet.at(entity);
    else
        return NullEntityIndex;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::clear(void) noexcept
{
    _indexSet.clearUnsafe();
    _entities.clear();
    _components.clear();
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::release(void) noexcept
{
    _indexSet.releaseUnsafe();
    _entities.release();
    _components.release();
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::ECS::EntityIndex kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::findIndex(const Entity entity) const noexcept
{
    const auto it = _entities.find(entity);
    if (it != _entities.end()) [[likely]]
        return static_cast<Entity>(std::distance(_entities.begin(), it));
    else [[unlikely]]
        return NullEntityIndex;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename CompareFunctor>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::sort(CompareFunctor &&compareFunc) noexcept
{
    // Sort entities
    std::sort(_entities.begin(), _entities.end(), std::forward<CompareFunctor>(compareFunc));

    // Apply sort patch to components & sparse set
    for (EntityIndex from {}, to = _entities.size(); from != to; ++from) {
        auto current = from;
        auto next = _indexSet.at(_entities.at(current));
        while (current != next) {
            const auto index = _indexSet.at(_entities.at(next));
            const auto entity = _entities.at(current);
            std::swap(atIndex(next), atIndex(index));
            _indexSet.at(entity) = current;
            current = std::exchange(next, index);
        }
    }
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Callback>
    requires std::is_invocable_v<Callback, ComponentType &>
        || std::is_invocable_v<Callback, kF::ECS::Entity>
        || std::is_invocable_v<Callback, kF::ECS::Entity, ComponentType &>
inline void kF::ECS::ComponentTable<ComponentType, EntityPageSize, Allocator>::traverse(Callback &&callback) noexcept
{
    for (EntityIndex index {}, count = _entities.size(); index != count; ++index) {
        // Entity & Component
        if constexpr (std::is_invocable_v<Callback, Entity, ComponentType &>) {
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, Entity, ComponentType &>, bool>) {
                if (!callback(_entities.at(index), _components.at(index)))
                    break;
            } else
                callback(_entities.at(index), _components.at(index));
        // Component only
        } else if constexpr (std::is_invocable_v<Callback, ComponentType &>) {
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, ComponentType &>, bool>) {
                if (!callback(_components.at(index)))
                    break;
            } else
                callback(_components.at(index));
        // Entity only
        } else {
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, Entity>, bool>) {
                if (!callback(_entities.at(index)))
                    break;
            } else
                callback(_entities.at(index));
        }
    }
}