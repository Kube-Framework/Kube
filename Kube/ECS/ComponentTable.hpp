/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#pragma once

#include <Kube/Core/SparseSet.hpp>

#include "Base.hpp"

namespace kF::ECS
{
    template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
    class ComponentTable;
}

template<typename ComponentType, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class alignas_cacheline kF::ECS::ComponentTable
{
public:
    /** @brief Is table stable ? */
    static constexpr bool IsStable = false;


    /** @brief ComponentType of stored component */
    using ValueType = ComponentType;

    /** @brief Sparse set that stores indexes of entities' components */
    using IndexSparseSet = Core::SparseSet<Entity, EntityPageSize, Allocator, EntityIndex, &Internal::EntityIndexInitializer>;

    /** @brief List of entities */
    using Entities = Core::Vector<Entity, Allocator, EntityIndex>;

    /** @brief List of entities' components */
    using Components = Core::Vector<ComponentType, Allocator, EntityIndex>;

    /** @brief Component iterator */
    using ComponentIterator = Components::Iterator;

    /** @brief Component const iterator */
    using ComponentConstIterator = Components::ConstIterator;

    /** @brief Component reverse iterator */
    using ComponentReverseIterator = Components::ReverseIterator;

    /** @brief Component const reverse iterator */
    using ComponentConstReverseIterator = Components::ConstReverseIterator;

    static_assert(IndexSparseSet::IsSafeToClear, "ECS::ComponentTable: There are no reason why index sparse set could not be safely cleared");
    static_assert(EntityPageSize != 0, "ECS::ComponentTable: Entity page size cannot be null");



    /** @brief Get the number of components inside the table */
    [[nodiscard]] inline EntityIndex count(void) const noexcept { return _entities.size(); }

    /** @brief Check if an entity exists in the sparse set */
    [[nodiscard]] inline bool exists(const Entity entity) const noexcept
        { return getUnstableIndex(entity) != NullEntityIndex; }


    /** @brief Add a component into the table */
    template<typename ...Args>
    ComponentType &add(const Entity entity, Args &&...args) noexcept;

    /** @brief Try to add a component into the table
     *  @note If the entity already attached 'ComponentType', the old value is updated */
    ComponentType &tryAdd(const Entity entity, ComponentType &&component) noexcept;

    /** @brief Try to update component of an entity
     *  @note If a component doesn't exists, it is created */
    template<typename Functor>
    ComponentType &tryAdd(const Entity entity, Functor &&functor) noexcept;

    /** @brief Add a range of components into the table */
    template<typename ...Args>
    void addRange(const EntityRange range, const Args &...args) noexcept;


    /** @brief Remove a component from the table
     *  @note The entity must be inside table else its an undefined behavior (use exists to check if an entity is registered) */
    void remove(const Entity entity) noexcept;

    /** @brief Try to remove a component from the table
     *  @note The entity can be inside table, if it isn't this function does nothing
     *  @return True if the component has been removed */
    bool tryRemove(const Entity entity) noexcept;

    /** @brief Remove a range of components from the table
     *  @note The range of entities can be inside table, if none are present this function does nothing */
    void removeRange(const EntityRange range) noexcept;


    /** @brief Extract and remove a component into the table
     *  @note The entity must be inside table else its an undefined behavior (use exists to check if an entity is registered) */
    [[nodiscard]] ComponentType extract(const Entity entity) noexcept;


    /** @brief Get an entity's component */
    [[nodiscard]] inline const ComponentType &get(const Entity entity) const noexcept
        { return _components.at(_indexSet.at(entity)); }
    [[nodiscard]] inline ComponentType &get(const Entity entity) noexcept
        { return const_cast<ComponentType &>(std::as_const(*this).get(entity)); }


    /** @brief Get the unstable index of an entity (NullEntityIndex if not found) */
    [[nodiscard]] EntityIndex getUnstableIndex(const Entity entity) const noexcept;

    /** @brief Get an entity's component using its unstable index */
    [[nodiscard]] inline ComponentType &atIndex(const EntityIndex entityIndex) noexcept
        { return _components.at(entityIndex); }
    [[nodiscard]] inline const ComponentType &atIndex(const EntityIndex entityIndex) const noexcept
        { return _components.at(entityIndex); }


    /** @brief Components begin / end iterators */
    [[nodiscard]] inline auto begin(void) noexcept { return _components.begin(); }
    [[nodiscard]] inline auto begin(void) const noexcept { return _components.begin(); }
    [[nodiscard]] inline auto cbegin(void) const noexcept { return _components.begin(); }
    [[nodiscard]] inline auto end(void) noexcept { return _components.end(); }
    [[nodiscard]] inline auto end(void) const noexcept { return _components.end(); }
    [[nodiscard]] inline auto cend(void) const noexcept { return _components.end(); }

    /** @brief Components reverse begin / end iterators */
    [[nodiscard]] inline auto rbegin(void) noexcept { return _components.rbegin(); }
    [[nodiscard]] inline auto rbegin(void) const noexcept { return _components.rbegin(); }
    [[nodiscard]] inline auto crbegin(void) const noexcept { return _components.rbegin(); }
    [[nodiscard]] inline auto rend(void) noexcept { return _components.rend(); }
    [[nodiscard]] inline auto rend(void) const noexcept { return _components.rend(); }
    [[nodiscard]] inline auto crend(void) const noexcept { return _components.rend(); }


    /** @brief Get registered entity list */
    [[nodiscard]] inline const auto &entities(void) const noexcept { return _entities; }


    /** @brief Sort the table using a custom functor
     *  @note CompareFunctor must have the following signature: bool(Entity, Entity) */
    template<typename CompareFunctor>
    void sort(CompareFunctor &&compareFunc) noexcept;


    /** @brief Clear the table */
    void clear(void) noexcept;

    /** @brief Release the table */
    void release(void) noexcept;


    /** @brief Traverse table with a callback taking (Entity, Component &) as arguments or only (Component &)
     *  @note If the callback returns a boolean, traversal is stopped when 'false' is returned */
    template<typename Callback>
        requires std::is_invocable_v<Callback, ComponentType &>
            || std::is_invocable_v<Callback, kF::ECS::Entity>
            || std::is_invocable_v<Callback, kF::ECS::Entity, ComponentType &>
    void traverse(Callback &&callback) noexcept;

    /** @brief Traverse table with a callback taking (Entity, const Component &) as arguments or only (const Component &)
     *  @note If the callback returns a boolean, traversal is stopped when 'false' is returned */
    template<typename Callback>
        requires std::is_invocable_v<Callback, const ComponentType &>
            || std::is_invocable_v<Callback, kF::ECS::Entity>
            || std::is_invocable_v<Callback, kF::ECS::Entity, const ComponentType &>
    inline void traverse(Callback &&callback) const noexcept
        { const_cast<ComponentTable &>(*this).traverse(std::forward<Callback>(callback)); }


    /** @brief Find an element by comparison */
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentIterator find(const Comparable &comparable) noexcept
        { return std::find(begin(), end(), comparable); }
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentConstIterator find(const Comparable &comparable) const noexcept
        { return std::find(begin(), end(), comparable); }

    /** @brief Find an element by comparison, using begin iterator */
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentIterator find(const ComponentIterator from, const Comparable &comparable) noexcept
        { return std::find(from, end(), comparable); }
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentConstIterator find(const ComponentConstIterator from, const Comparable &comparable) const noexcept
        { return std::find(from, end(), comparable); }

    /** @brief Find an element by comparison with reverse order */
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentReverseIterator rfind(const Comparable &comparable) noexcept
        { return std::find(rbegin(), rend(), comparable); }
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentConstReverseIterator rfind(const Comparable &comparable) const noexcept
        { return std::find(rbegin(), rend(), comparable); }

    /** @brief Find an element by comparison with reverse order, using begin iterator */
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentReverseIterator rfind(const ComponentReverseIterator from, const Comparable &comparable) noexcept
        { return std::find(from, rend(), comparable); }
    template<typename Comparable>
        requires requires(const ComponentType &lhs, const Comparable &rhs) { lhs == rhs; }
    [[nodiscard]] inline ComponentConstReverseIterator rfind(const ComponentConstReverseIterator from, const Comparable &comparable) const noexcept
        { return std::find(from, rend(), comparable); }

    /** @brief Find an element with functor */
    template<typename Functor>
        requires std::invocable<Functor, ComponentType &>
    [[nodiscard]] inline ComponentIterator find(Functor &&functor) noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const ComponentType &>
    [[nodiscard]] inline ComponentConstIterator find(Functor &&functor) const noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor, using begin iterator */
    template<typename Functor>
        requires std::invocable<Functor, ComponentType &>
    [[nodiscard]] inline ComponentIterator find(const ComponentIterator from, Functor &&functor) noexcept
        { return std::find_if(from, end(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const ComponentType &>
    [[nodiscard]] inline ComponentConstIterator find(const ComponentConstIterator from, Functor &&functor) const noexcept
        { return std::find_if(from, end(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor with reverse order */
    template<typename Functor>
        requires std::invocable<Functor, ComponentType &>
    [[nodiscard]] inline ComponentReverseIterator rfind(Functor &&functor) noexcept
        { return std::find_if(rbegin(), rend(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const ComponentType &>
    [[nodiscard]] inline ComponentConstReverseIterator rfind(Functor &&functor) const noexcept
        { return std::find_if(rbegin(), rend(), std::forward<Functor>(functor)); }

    /** @brief Find an element with functor with reverse order, using reversed begin iterator */
    template<typename Functor>
        requires std::invocable<Functor, ComponentType &>
    [[nodiscard]] inline ComponentReverseIterator rfind(const ComponentReverseIterator from, Functor &&functor) noexcept
        { return std::find_if(from, rend(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const ComponentType &>
    [[nodiscard]] inline ComponentConstReverseIterator rfind(const ComponentConstReverseIterator from, Functor &&functor) const noexcept
        { return std::find_if(from, rend(), std::forward<Functor>(functor)); }

private:
    /** @brief Check if an entity exists in the sparse set */
    [[nodiscard]] EntityIndex findIndex(const Entity entity) const noexcept;


    /** @brief Hiden implementation of add function */
    template<typename ...Args>
    ComponentType &addImpl(const Entity entity, Args &&...args) noexcept;


    /** @brief Hiden implementation of remove function */
    void removeImpl(const Entity entity, const EntityIndex entityIndex) noexcept;


    IndexSparseSet _indexSet {};
    Entities _entities {};
    Components _components {};
};

#include "ComponentTable.ipp"
