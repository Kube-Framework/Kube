/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#include <Kube/Core/SmallVector.hpp>

#include "StableComponentTable.hpp"

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Type>
inline kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::IteratorType<Type> &
    kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::IteratorType<Type>::operator++(void) noexcept
{
    const auto &entities = _table->entities();
    const auto max = entities.size();

    while (true) {
        if (_index == max)
            break;
        ++_index;
        if (entities.at(_index) != NullEntityIndex)
            break;
    }
    return *this;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Type>
inline kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::IteratorType<Type> &
    kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::IteratorType<Type>::operator--(void) noexcept
{
    const auto &entities = _table->entities();
    const auto max = entities.size();

    while (true) {
        if (_index == 0u)
            break;
        --_index;
        if (entities.at(_index) != NullEntityIndex)
            break;
    }
    return *this;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::~StableComponentTable(void) noexcept
{
    destroyComponents();
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::pack(void) noexcept
{
    // If there are no component to pack, return now
    if (_tombstones.empty()) [[likely]]
        return;

    auto last = _entities.size();
    while (last && _entities.at(last - 1u) == NullEntity)
        --last;
    for (auto it = _tombstones.begin(), end = _tombstones.end(); it != end && last; ++it) {
        if (*it >= last)
            continue;
        --last;
        Entity &target = _entities.at(*it);
        target = _entities.at(last);
        auto &componentTarget = atIndex(last);
        insertComponent(*it, std::move(componentTarget));
        if constexpr (!std::is_trivially_destructible_v<ComponentType>) {
            componentTarget.~ComponentType();
        }
        _indexSet.at(target) = *it;
        while (last && _entities.at(last - 1u) == NullEntity)
            --last;
    }
    _tombstones.clear();
    _entities.erase(_entities.begin() + last, _entities.end());
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::add(const Entity entity, Args &&...args) noexcept
{
    kFAssert(!exists(entity),
        "ECS::StableComponentTable::add: Entity '", entity, "' already exists");

    return addImpl(entity, std::forward<Args>(args)...);
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::tryAdd(const Entity entity, ComponentType &&component) noexcept
{
    if (auto componentIndex = findIndex(entity); componentIndex != NullEntityIndex) [[likely]] {
        return get(entity) = std::move(component);
    } else {
        return addImpl(entity, std::move(component));
    }
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Functor>
inline ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::tryAdd(const Entity entity, Functor &&functor) noexcept
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

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::addRange(const EntityRange range, const Args &...args) noexcept
{
    const auto lastIndex = _entities.size();
    const auto count = range.end - range.begin;

    if constexpr (KUBE_DEBUG_BUILD) {
        // Ensure no entity exists in range
        for (const auto entity : _entities) {
            kFEnsure(entity < range.begin || entity >= range.end,
                "ECS::StableComponentTable::addRange: Entity '", entity, "' from entity range [", range.begin, ", ", range.end, "[ already exists");
        }
    }

    // Insert entities
    _entities.insertCustom(_entities.end(), count, [range](const auto count, const auto out) {
        for (EntityIndex i = 0; i != count; ++i)
            out[i] = range.begin + i;
    });

    // Insert indexes
    for (EntityIndex i = lastIndex, it = range.begin; it != range.end; ++i, ++it) {
        _indexSet.add(it, i);
    }

    // Insert components
    for (EntityIndex i = lastIndex, it = range.begin; it != range.end; ++i, ++it) {
        insertComponent(i, args...);
    }
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::addImpl(const Entity entity, Args &&...args) noexcept
{
    // Find a suitable component index
    ECS::EntityIndex entityIndex;
    if (_tombstones.empty()) {
        entityIndex = _entities.size();
        _entities.push(entity);
    } else {
        entityIndex = _tombstones.back();
        _tombstones.pop();
        _entities.at(entityIndex) = entity;
    }
    // Insert at index
    _indexSet.add(entity, entityIndex);
    return insertComponent(entityIndex, std::forward<Args>(args)...);
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename ...Args>
inline ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::insertComponent(const EntityIndex entityIndex, Args &&...args) noexcept
{
    const auto pageIndex = GetPageIndex(entityIndex);
    const auto componentIndex = GetComponentIndex(entityIndex);

    // Ensure destination page exists
    while (!pageExists(pageIndex)) [[unlikely]]
        _componentPages.push(ComponentPagePtr::Make());
    // Construct component
    return *new (_componentPages.at(pageIndex)->data() + componentIndex) ComponentType(std::forward<Args>(args)...);
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::remove(const Entity entity) noexcept
{
    kFAssert(exists(entity),
        "ECS::StableComponentTable::remove: Entity '", entity, "' doesn't exists");
    removeImpl(entity, _indexSet.extract(entity));
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline bool kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::tryRemove(const Entity entity) noexcept
{
    if (const auto entityIndex = findIndex(entity); entityIndex != NullEntityIndex) [[likely]] {
        removeImpl(entity, entityIndex);
        return true;
    } else
        return false;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::removeRange(const EntityRange range) noexcept
{
    for (auto it = range.begin; it != range.end; ++it)
        removeImpl(it, _indexSet.at(it));
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::removeImpl(const Entity entity, const EntityIndex entityIndex) noexcept
{
    // Remove component
    if constexpr (!std::is_trivially_destructible_v<ComponentType>) {
        const auto pageIndex = GetPageIndex(entityIndex);
        const auto componentIndex = GetComponentIndex(entityIndex);
        _componentPages.at(pageIndex)->data()[componentIndex].~ComponentType();
    }

    // Remove index
    _indexSet.remove(entity);

    // Remove entity
    _entities.at(entityIndex) = NullEntity;

    // Add tombstone
    _tombstones.push(entityIndex);
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline ComponentType kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::extract(const Entity entity) noexcept
{
    kFAssert(exists(entity),
        "ECS::StableComponentTable::remove: Entity '", entity, "' doesn't exists");

    const auto entityIndex = _indexSet.extract(entity);
    ComponentType value(std::move(atIndex(entityIndex)));

    removeImpl(entity, entityIndex);
    return value;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline const ComponentType &kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::atIndex(const EntityIndex entityIndex) const noexcept
{
    const auto pageIndex = GetPageIndex(entityIndex);
    const auto componentIndex = GetComponentIndex(entityIndex);

    return _componentPages.at(pageIndex)->data()[componentIndex];
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::ECS::EntityIndex kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::getUnstableIndex(const Entity entity) const noexcept
{
    if (_indexSet.pageExists(entity)) [[likely]]
        return _indexSet.at(entity);
    else
        return NullEntityIndex;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::clear(void) noexcept
{
    destroyComponents();
    _entities.clear();
    _indexSet.clearUnsafe();
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::release(void) noexcept
{
    destroyComponents();
    _componentPages.release();
    _entities.release();
    _indexSet.releaseUnsafe();
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::destroyComponents(void) noexcept
{
    if constexpr (!std::is_trivially_destructible_v<ComponentType>) {
        for (ECS::EntityIndex index {}; const auto entity : _entities) {
            if (entity != ECS::NullEntity) [[likely]]
                atIndex(index).~ComponentType();
            ++index;
        }
    }
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
inline kF::ECS::EntityIndex kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::findIndex(const Entity entity) const noexcept
{
    const auto it = _entities.find(entity);
    if (it != _entities.end()) [[likely]]
        return static_cast<Entity>(std::distance(_entities.begin(), it));
    else [[unlikely]]
        return NullEntityIndex;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename CompareFunctor>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::sort(CompareFunctor &&compareFunc) noexcept
{
    // Pack before sorting
    pack();

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

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
template<typename Callback>
    requires std::is_invocable_v<Callback, ComponentType &>
        || std::is_invocable_v<Callback, kF::ECS::Entity>
        || std::is_invocable_v<Callback, kF::ECS::Entity, ComponentType &>
inline void kF::ECS::StableComponentTable<ComponentType, ComponentPageSize, EntityPageSize, Allocator>::traverse(Callback &&callback) noexcept
{
    for (EntityIndex index {}; index < _entities.size(); ++index) {
        const auto entity = _entities[index];
        if (entity == NullEntity) [[unlikely]]
            continue;
        // Entity & component
        if constexpr (std::is_invocable_v<Callback, Entity, ComponentType &>) {
            auto &component = atIndex(index);
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, Entity, ComponentType &>, bool>) {
                if (!callback(entity, component))
                    break;
            } else
                callback(entity, component);
        // Component only
        } else if constexpr (std::is_invocable_v<Callback, ComponentType &>) {
            auto &component = atIndex(index);
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, ComponentType &>, bool>) {
                if (!callback(component))
                    break;
            } else
                callback(component);
        // Entity only
        } else {
            if constexpr (std::is_same_v<std::invoke_result_t<Callback, Entity>, bool>) {
                if (!callback(entity))
                    break;
            } else
                callback(entity);
        }
    }
}