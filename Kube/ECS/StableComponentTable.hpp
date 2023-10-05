/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#pragma once

#include <Kube/Core/SparseSet.hpp>

#include "Base.hpp"

namespace kF::ECS
{
    template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize, kF::Core::StaticAllocatorRequirements Allocator>
    class StableComponentTable;
}

template<typename ComponentType, kF::ECS::EntityIndex ComponentPageSize, kF::ECS::EntityIndex EntityPageSize,
        kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class alignas_cacheline kF::ECS::StableComponentTable
{
public:
    /** @brief Is table stable ? */
    static constexpr bool IsStable = true;


    /** @brief Type of stored component */
    using ValueType = ComponentType;

    /** @brief Sparse set that stores indexes of entities' components */
    using IndexSparseSet = Core::SparseSet<Entity, EntityPageSize, Allocator, EntityIndex, &Internal::EntityIndexInitializer>;

    /** @brief List of entities */
    using Entities = Core::Vector<Entity, Allocator, EntityIndex>;

    /** @brief List of entity indexes */
    using EntityIndexes = Core::Vector<EntityIndex, Allocator, EntityIndex>;

    /** @brief Page of entities' component */
    struct alignas(alignof(ComponentType)) ComponentPage
    {
        std::byte opaque[ComponentPageSize * sizeof(ComponentType)] {};

        /** @brief Get component data pointer */
        [[nodiscard]] inline ComponentType *data(void) noexcept { return reinterpret_cast<ComponentType *>(opaque); }

        /** @brief Get component data pointer */
        [[nodiscard]] inline const ComponentType *data(void) const noexcept { return reinterpret_cast<const ComponentType *>(opaque); }
    };

    /** @brief Unique pointer to component page */
    using ComponentPagePtr = Core::UniquePtr<ComponentPage, Allocator>;

    /** @brief A list of component pages */
    using ComponentPages = Core::Vector<ComponentPagePtr, Allocator, EntityIndex>;

    static_assert(IndexSparseSet::IsSafeToClear, "ECS::StableComponentTable: There are no reason why index sparse set could not be safely cleared");
    static_assert(EntityPageSize != 0, "ECS::StableComponentTable: Entity page size cannot be null");
    static_assert(ComponentPageSize != 0, "ECS::StableComponentTable: Component page size cannot be null");
    static_assert(Core::IsPowerOf2(ComponentPageSize), "ECS::StableComponentTable: Component page size must be a power of 2");

    /** @brief Iterator abstraction */
    template<typename Type>
    struct IteratorType
    {
    public:
        /** @brief Destructor */
        inline ~IteratorType(void) noexcept = default;

        /** @brief Value constructor */
        inline IteratorType(StableComponentTable * const table, const EntityIndex index) noexcept
            : _table(table), _index(index) {}

        /** @brief Copy constructor */
        inline IteratorType(const IteratorType &other) noexcept = default;

        /** @brief Copy assignment */
        inline IteratorType &operator=(const IteratorType &other) noexcept = default;


        /** @brief Dereference iterator */
        [[nodiscard]] inline Type &operator*(void) const noexcept { return _table->atIndex(_index); }

        /** @brief Dereference iterator */
        [[nodiscard]] inline Type &operator->(void) const noexcept { return _table->atIndex(_index); }


        /** @brief Prefix increment operator */
        IteratorType &operator++(void) noexcept;

        /** @brief Postfix increment operator */
        [[nodiscard]] inline IteratorType operator++(int) noexcept { const auto past = *this; ++*this; return past; }

        /** @brief Prefix decrement operator */
        IteratorType &operator--(void) noexcept;

        /** @brief Postfix decrement operator */
        [[nodiscard]] inline IteratorType operator--(int) noexcept { const auto past = *this; --*this; return past; }


        /** @brief Equal operators */
        [[nodiscard]] inline auto operator==(const IteratorType &other) const noexcept
            { return _table == other._table && _index == other._index; }
        [[nodiscard]] inline auto operator!=(const IteratorType &other) const noexcept
            { return _table != other._table || _index != other._index; }

    private:
        StableComponentTable *_table {};
        EntityIndex _index {};
    };

    /** @brief Mutable iterator */
    using Iterator = IteratorType<ValueType>;

    /** @brief Immutable iterator */
    using ConstIterator = IteratorType<const ValueType>;


    /** @brief Destructor*/
    ~StableComponentTable(void) noexcept;


    /** @brief Get the number of components inside the table */
    [[nodiscard]] inline EntityIndex count(void) const noexcept { return _entities.size() - _tombstones.size(); }

    /** @brief Check if an entity exists in the sparse set */
    [[nodiscard]] inline bool exists(const Entity entity) const noexcept
        { return getUnstableIndex(entity) != NullEntityIndex; }


    /** @brief Pack all components in memory (will break pointer stability and sort order) */
    void pack(void) noexcept;


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
        { return atIndex(_indexSet.at(entity)); }
    [[nodiscard]] inline ComponentType &get(const Entity entity) noexcept
        { return const_cast<ComponentType &>(std::as_const(*this).get(entity)); }


    /** @brief Get the unstable index of an entity (NullEntityIndex if not found) */
    [[nodiscard]] EntityIndex getUnstableIndex(const Entity entity) const noexcept;

    /** @brief Get an entity's component using its unstable index */
    [[nodiscard]] inline const ComponentType &atIndex(const EntityIndex entityIndex) const noexcept;
    [[nodiscard]] inline ComponentType &atIndex(const EntityIndex entityIndex) noexcept
        { return const_cast<ComponentType &>(std::as_const(*this).atIndex(entityIndex)); }


    /** @brief Components begin / end iterators */
    [[nodiscard]] inline auto begin(void) noexcept { return Iterator(this, 0u); }
    [[nodiscard]] inline auto begin(void) const noexcept { return ConstIterator(this, 0u); }
    [[nodiscard]] inline auto cbegin(void) const noexcept { return ConstIterator(this, 0u); }
    [[nodiscard]] inline auto end(void) noexcept { return Iterator(this, count()); }
    [[nodiscard]] inline auto end(void) const noexcept { return ConstIterator(this, count()); }
    [[nodiscard]] inline auto cend(void) const noexcept { return ConstIterator(this, count()); }

    /** @brief Components reverse begin / end iterators */
    [[nodiscard]] inline auto rbegin(void) noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] inline auto rbegin(void) const noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] inline auto crbegin(void) const noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] inline auto rend(void) noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] inline auto rend(void) const noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] inline auto crend(void) const noexcept { return std::make_reverse_iterator(end()); }


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
        { const_cast<StableComponentTable &>(*this).traverse(std::forward<Callback>(callback)); }

private:
    /** @brief Check if an entity exists in the sparse set */
    [[nodiscard]] EntityIndex findIndex(const Entity entity) const noexcept;


    /** @brief Hiden implementation of add function */
    template<typename ...Args>
    ComponentType &addImpl(const Entity entity, Args &&...args) noexcept;

    /** @brief Insert a component into page at entityIndex */
    template<typename ...Args>
    ComponentType &insertComponent(const EntityIndex entityIndex, Args &&...args) noexcept;


    /** @brief Hiden implementation of remove function */
    void removeImpl(const Entity entity, const EntityIndex entityIndex) noexcept;


    /** @brief Destroy all components */
    void destroyComponents(void) noexcept;


    /** @brief Check if a page exists at index */
    [[nodiscard]] bool pageExists(const EntityIndex pageIndex) const noexcept
        { return pageIndex < _componentPages.size(); }


    /** @brief Get page index from an entity index */
    [[nodiscard]] static constexpr EntityIndex GetPageIndex(const EntityIndex unstableIndex) noexcept
        { return unstableIndex / ComponentPageSize; }

    /** @brief Get component index from an entity index */
    [[nodiscard]] static constexpr EntityIndex GetComponentIndex(const EntityIndex unstableIndex) noexcept
        { return unstableIndex & (ComponentPageSize - 1); }


    IndexSparseSet _indexSet {};
    Entities _entities {};
    EntityIndexes _tombstones {};
    ComponentPages _componentPages {};
};

#include "StableComponentTable.ipp"
