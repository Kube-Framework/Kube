/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of System
 */

#include <gtest/gtest.h>

#include <Kube/Core/Random.hpp>

#include <Kube/ECS/ComponentTable.hpp>
#include <Kube/ECS/StableComponentTable.hpp>

using namespace kF;

using TestComponent = std::unique_ptr<int>;

template<typename TableType>
void TestTableBasics(void) noexcept
{
    TableType table;

    ASSERT_EQ(table.count(), 0u);
}

template<typename TableType>
void TestTableAddRemove(void) noexcept
{
    static constexpr int TestValue = 42;
    static constexpr ECS::Entity TestEntity = 1u;

    TableType table;

    // Add
    ASSERT_EQ(table.count(), 0u);
    auto &component = table.add(TestEntity, std::make_unique<int>(TestValue));
    ASSERT_EQ(table.count(), 1u);

    // Test insertion
    ASSERT_TRUE(component);
    ASSERT_EQ(*component, TestValue);
    ASSERT_TRUE(table.exists(TestEntity));
    ASSERT_EQ(&table.get(TestEntity), &component);

    // Remove
    table.remove(TestEntity);
    ASSERT_EQ(table.count(), 0u);

    // Test remove
    ASSERT_FALSE(table.exists(TestEntity));
}

template<typename TableType>
void TestTableAddRemoveRange(void) noexcept
{
    static constexpr ECS::EntityRange TestEntityRange { 0u, 100u };

    TableType table;

    // Add range
    ASSERT_EQ(table.count(), 0u);
    table.addRange(TestEntityRange);
    ASSERT_EQ(table.count(), TestEntityRange.size());

    // Test insertion
    for (auto it = TestEntityRange.begin; it != TestEntityRange.end; ++it) {
        ASSERT_TRUE(table.exists(it));
    }

    // Remove range
    table.removeRange(TestEntityRange);
    ASSERT_EQ(table.count(), 0u);

    // Test remove
    for (auto it = TestEntityRange.begin; it != TestEntityRange.end; ++it) {
        ASSERT_FALSE(table.exists(it));
    }
}

template<typename TableType>
void TestTableTryAddRemove(void) noexcept
{
    static constexpr int TestValue = 42;
    static constexpr int TestValue2 = 24;
    static constexpr int TestValue3 = 123;
    static constexpr ECS::Entity TestEntity = 1u;

    TableType table;

    // TryRemove (not existing)
    ASSERT_FALSE(table.tryRemove(TestEntity));

    // TryAdd (not existing)
    ASSERT_EQ(table.count(), 0u);
    auto &component = table.tryAdd(TestEntity, std::make_unique<int>(TestValue));
    ASSERT_EQ(table.count(), 1u);

    // Test insertion
    ASSERT_TRUE(component);
    ASSERT_EQ(*component, TestValue);
    ASSERT_TRUE(table.exists(TestEntity));
    ASSERT_EQ(&table.get(TestEntity), &component);

    // TryAdd (existing, replace)
    ASSERT_EQ(table.count(), 1u);
    auto &component2 = table.tryAdd(TestEntity, std::make_unique<int>(TestValue2));
    ASSERT_EQ(table.count(), 1u);

    // Test update
    ASSERT_TRUE(component2);
    ASSERT_EQ(&component, &component2);
    ASSERT_EQ(*component2, TestValue2);
    ASSERT_TRUE(table.exists(TestEntity));
    ASSERT_EQ(&table.get(TestEntity), &component2);

    // TryAdd (existing, modify)
    ASSERT_EQ(table.count(), 1u);
    auto &component3 = table.tryAdd(TestEntity, [](auto &component) { component = std::make_unique<int>(TestValue3); });
    ASSERT_EQ(table.count(), 1u);

    // Test update
    ASSERT_TRUE(component3);
    ASSERT_EQ(&component, &component3);
    ASSERT_EQ(*component3, TestValue3);
    ASSERT_TRUE(table.exists(TestEntity));
    ASSERT_EQ(&table.get(TestEntity), &component3);

    // TryRemove (existing)
    ASSERT_TRUE(table.tryRemove(TestEntity));

    // Test remove
    ASSERT_FALSE(table.exists(TestEntity));

    // TryRemove (not existing)
    ASSERT_FALSE(table.tryRemove(TestEntity));
}

template<typename TableType>
void TestTableExtract(void) noexcept
{
    static constexpr int TestValue = 42;
    static constexpr ECS::Entity TestEntity = 1u;

    TableType table;

    // Add
    ASSERT_EQ(table.count(), 0u);
    auto &component = table.add(TestEntity, std::make_unique<int>(TestValue));
    ASSERT_EQ(table.count(), 1u);

    // Test insertion
    ASSERT_TRUE(component);
    ASSERT_EQ(*component, TestValue);
    ASSERT_TRUE(table.exists(TestEntity));
    ASSERT_EQ(&table.get(TestEntity), &component);

    // Extract
    auto value = table.extract(TestEntity);
    ASSERT_EQ(table.count(), 0u);
    ASSERT_EQ(*value, TestValue);

    // Test remove
    ASSERT_FALSE(table.exists(TestEntity));
}

template<typename TableType>
void TestTableSort(void) noexcept
{
    static constexpr ECS::EntityIndex EntityCount = 100u;

    TableType table;
    ECS::Entity entity {};
    auto value = static_cast<int>(EntityCount);

    // Add some values
    for (auto i = 0u; i < EntityCount; ++i)
        table.add(++entity, std::make_unique<int>(--value));
    ASSERT_EQ(table.count(), EntityCount);

    // Sort
    table.sort([&table](const ECS::Entity lhs, const ECS::Entity rhs) {
        return *table.get(lhs) < *table.get(rhs);
    });

    // Test sort
    int lastValue {};
    for (auto index = 0; auto &component : table) {
        if (index++) {
            ASSERT_GT(*component, lastValue);
        }
        lastValue = *component;
    }
}

template<typename TableType>
void TestTableTraverse(void) noexcept
{
    static constexpr ECS::EntityIndex EntityCount = 100u;

    TableType table;
    ECS::Entity entity {};
    int value {};

    for (auto i = 0u; i < EntityCount; ++i)
        table.add(++entity, std::make_unique<int>(++value));
    ASSERT_EQ(table.count(), EntityCount);

    // Entity & Component
    bool error {};
    table.traverse([&error, e = 0u, i = 0](const ECS::Entity entity, const auto &component) mutable {
        error = ++e != entity || ++i != *component;
    });
    ASSERT_FALSE(error);

    // Component only
    table.traverse([&error, i = 0](const auto &component) mutable {
        error = ++i != *component;
    });
    ASSERT_FALSE(error);

    // Entity only
    table.traverse([&error, e = 0u](const auto entity) mutable {
        error = ++e != entity;
    });
    ASSERT_FALSE(error);


    // Entity & Component (return)
    ECS::EntityIndex count {};
    table.traverse([&error, &count, e = 0u, i = 0](const ECS::Entity entity, const auto &component) mutable {
        error = ++e != entity || ++i != *component;
        return ++count != (EntityCount / 2);
    });
    ASSERT_FALSE(error);
    ASSERT_EQ(count, EntityCount / 2);

    // Component (return)
    count = {};
    table.traverse([&error, &count, i = 0](const auto &component) mutable {
        error = ++i != *component;
        return ++count != (EntityCount / 2);
    });
    ASSERT_FALSE(error);
    ASSERT_EQ(count, EntityCount / 2);

    // Entity (return)
    count = {};
    table.traverse([&error, &count, e = 0u](const auto entity) mutable {
        error = ++e != entity;
        return ++count != (EntityCount / 2);
    });
    ASSERT_FALSE(error);
    ASSERT_EQ(count, EntityCount / 2);
}

template<typename TableType>
void TestTableClear(void) noexcept
{
    static constexpr ECS::EntityRange TestEntityRange { 0u, 100u };

    TableType table;

    // Add range
    ASSERT_EQ(table.count(), 0u);
    table.addRange(TestEntityRange);
    ASSERT_EQ(table.count(), TestEntityRange.size());

    // Clear table
    table.clear();
    ASSERT_EQ(table.count(), 0u);
}

template<typename TableType>
void TestTableRelease(void) noexcept
{
    static constexpr ECS::EntityRange TestEntityRange { 0u, 100u };

    TableType table;

    // Add range
    ASSERT_EQ(table.count(), 0u);
    table.addRange(TestEntityRange);
    ASSERT_EQ(table.count(), TestEntityRange.size());

    // Release table
    table.release();
    ASSERT_EQ(table.count(), 0u);
}

template<typename TableType>
void TestTableSortBug01(void) noexcept
{
    static constexpr ECS::Entity Before[] {
        102, 101, 100, 99, 98, 90, 89, 88, 85, 222, 80, 210, 77, 75, 198, 71, 186, 68, 174, 65, 63, 162, 59, 150, 56, 138, 53, 51, 126, 47, 114, 44, 42, 37, 33, 31, 29, 21, 17, 15, 13, 5, 229, 230, 240, 242, 243, 244, 237, 247, 248, 249, 254, 255, 256, 257, 258, 269, 278, 286, 290, 293, 297, 298, 299, 300, 301, 302, 303, 304
    };

    static constexpr ECS::Entity After[] {
        293, 290, 286, 258, 257, 256, 255, 254, 249, 248, 247, 244, 243, 242, 240, 237, 304, 303, 302, 301, 300, 299, 298, 297, 278, 269, 230, 229, 102, 101, 100, 99, 98, 90, 89, 88, 85, 222, 80, 210, 77, 75, 198, 71, 186, 68, 174, 65, 63, 162, 59, 150, 56, 138, 53, 51, 126, 47, 114, 44, 42, 37, 33, 31, 29, 21, 17, 15, 13, 5
    };

    std::vector<int *> datas {};

    static constexpr auto SortFunc = [](const auto lhs, const auto rhs) {
        const auto lhsIt = std::find(std::begin(After), std::end(After), lhs);
        const auto rhsIt = std::find(std::begin(After), std::end(After), rhs);
        return lhsIt < rhsIt;
    };

    TableType table;

    for (const auto entity : Before) {
        const auto &component = table.add(entity, std::make_unique<int>(entity));
        datas.push_back(component.get());
    }

    table.traverse([](const ECS::Entity entity, const TestComponent &component) {
        ASSERT_EQ(entity, *component);
    });

    table.traverse([&](const ECS::Entity entity) {
        ASSERT_EQ(entity, *table.get(entity));
    });

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(entity, Before[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : Before) {
        ASSERT_EQ(table.get(entity).get(), datas[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(i, table.getUnstableIndex(entity));
        ++i;
    }

    table.sort(SortFunc);

    table.traverse([](const ECS::Entity entity, const TestComponent &component) {
        ASSERT_EQ(entity, *component);
    });

    table.traverse([&](const ECS::Entity entity) {
        ASSERT_EQ(entity, *table.get(entity));
    });

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(entity, After[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : Before) {
        ASSERT_EQ(table.get(entity).get(), datas[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(i, table.getUnstableIndex(entity));
        ++i;
    }
}

template<typename TableType>
void TestTableSortBug02(void) noexcept
{
    static constexpr ECS::Entity Before[] {
        3, 4, 0, 1, 2
    };

    static constexpr ECS::Entity After[] {
        0, 1, 2, 3, 4
    };

    std::vector<int *> datas {};

    static constexpr auto SortFunc = [](const auto lhs, const auto rhs) {
        const auto lhsIt = std::find(std::begin(After), std::end(After), lhs);
        const auto rhsIt = std::find(std::begin(After), std::end(After), rhs);
        return lhsIt < rhsIt;
    };

    TableType table;

    for (const auto entity : Before) {
        const auto &component = table.add(entity, std::make_unique<int>(entity));
        datas.push_back(component.get());
    }

    table.traverse([](const ECS::Entity entity, const TestComponent &component) {
        ASSERT_EQ(entity, *component);
    });

    table.traverse([&](const ECS::Entity entity) {
        ASSERT_EQ(entity, *table.get(entity));
    });

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(entity, Before[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : Before) {
        ASSERT_EQ(table.get(entity).get(), datas[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(i, table.getUnstableIndex(entity));
        ++i;
    }

    table.sort(SortFunc);

    table.traverse([](const ECS::Entity entity, const TestComponent &component) {
        ASSERT_EQ(entity, *component);
    });

    table.traverse([&](const ECS::Entity entity) {
        ASSERT_EQ(entity, *table.get(entity));
    });

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(entity, After[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : Before) {
        ASSERT_EQ(table.get(entity).get(), datas[i]);
        ++i;
    }

    for (auto i = 0u; const auto entity : table.entities()) {
        ASSERT_EQ(i, table.getUnstableIndex(entity));
        ++i;
    }
}


#define TEST_COMPONENT_TABLE(TableName, TableType) \
TEST(TableName, Basics) { TestTableBasics<TableType>(); } \
TEST(TableName, AddRemove) { TestTableAddRemove<TableType>(); } \
TEST(TableName, AddRemoveRange) { TestTableAddRemoveRange<TableType>(); } \
TEST(TableName, TryAddRemove) { TestTableTryAddRemove<TableType>(); } \
TEST(TableName, Extract) { TestTableExtract<TableType>(); } \
TEST(TableName, Sort) { TestTableSort<TableType>(); } \
TEST(TableName, SortBug01) { TestTableSortBug01<TableType>(); } \
TEST(TableName, SortBug02) { TestTableSortBug02<TableType>(); } \
TEST(TableName, Traverse) { TestTableTraverse<TableType>(); } \
TEST(TableName, Clear) { TestTableClear<TableType>(); } \
TEST(TableName, Release) { TestTableRelease<TableType>(); }

using ComponentTableType = ECS::ComponentTable<TestComponent, 4096 / sizeof(ECS::Entity)>;
using StableComponentTableType = ECS::StableComponentTable<TestComponent, 4096 / sizeof(ECS::Entity), 4096 / sizeof(TestComponent)>;

TEST_COMPONENT_TABLE(ComponentTable, ComponentTableType)
TEST_COMPONENT_TABLE(StableComponentTable, StableComponentTableType)

TEST(StableComponentTable, PackSparseHoles)
{
    static constexpr ECS::EntityRange TestEntityRange { 0u, 100u };

    StableComponentTableType table;

    // Add range
    ASSERT_EQ(table.count(), 0u);
    table.addRange(TestEntityRange);
    ASSERT_EQ(table.count(), TestEntityRange.size());

    // Remove some components
    table.remove(0u);
    table.remove(TestEntityRange.end / 8);
    table.remove(TestEntityRange.end / 4);
    table.remove(TestEntityRange.end / 2);
    table.remove(TestEntityRange.end - 1);

    // Test remove
    const ECS::Entity removeCount = 5;
    const ECS::Entity count = TestEntityRange.size() - removeCount;
    ASSERT_EQ(table.count(), count);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), removeCount);

    // Pack
    table.pack();

    // Test pack
    ASSERT_EQ(table.count(), count);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), 0);
}

TEST(StableComponentTable, PackBigHole)
{
    static constexpr ECS::EntityRange TestEntityRange { 0u, 100u };

    StableComponentTableType table;

    // Add range
    ASSERT_EQ(table.count(), 0u);
    table.addRange(TestEntityRange);
    ASSERT_EQ(table.count(), TestEntityRange.size());

    // Remove some components
    const ECS::Entity removeCount = (TestEntityRange.end - TestEntityRange.begin) / 4;
    table.removeRange(ECS::EntityRange { TestEntityRange.begin, TestEntityRange.begin + removeCount });

    // Test remove
    ASSERT_EQ(table.count(), TestEntityRange.size() - removeCount);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), removeCount);

    // Pack
    table.pack();

    // Test pack
    ASSERT_EQ(table.count(), TestEntityRange.size() - removeCount);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), 0);

    // Remove some components
    table.removeRange(ECS::EntityRange { TestEntityRange.begin + removeCount, TestEntityRange.begin + removeCount * 2 });

    // Test remove
    ASSERT_EQ(table.count(), TestEntityRange.size() - removeCount * 2);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), removeCount);

    // Pack
    table.pack();

    // Test pack
    ASSERT_EQ(table.count(), TestEntityRange.size() - removeCount * 2);
    ASSERT_EQ(std::count(table.entities().begin(), table.entities().end(), ECS::NullEntity), 0);
}

TEST(StableComponentTable, PackBug01)
{
    constexpr ECS::Entity Entities[] {
        230, 231, 241, 243, 244, 245, 238, 248, 249, 250, 261, 270, 279, 283, 286, 288,
        4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295,
        103, 102, 101, 100, 99, 91, 90, 89, 86, 223, 80, 211, 77, 75, 199, 71, 187, 68, 175, 65, 63, 163,
        59, 151, 56, 139, 53, 51, 127, 47, 115, 44, 42, 37, 33, 31, 29, 21, 17, 15, 13, 5
    };
    constexpr ECS::EntityIndex Tombstones[] {
        23, 22, 21, 20, 19, 18, 17, 16
    };

    StableComponentTableType table;

    // Add
    auto removeCount = 0;
    for (auto entity : Entities) {
        if (entity != ECS::NullEntity)
            table.add(entity, std::make_unique<int>(static_cast<int>(entity)));
        else {
            const auto toAdd = ECS::NullEntity - ++removeCount;
            table.add(ECS::NullEntity - ++removeCount, std::make_unique<int>(static_cast<int>(toAdd)));
        }
    }
    ASSERT_EQ(table.count(), std::size(Entities));

    // Remove
    for (auto tombstone : Tombstones) {
        table.remove(table.entities().at(tombstone));
    }
    ASSERT_EQ(table.count(), std::size(Entities) - std::size(Tombstones));
    ASSERT_EQ(table.entities().size(), std::size(Entities));

    // Pack
    table.pack();
    ASSERT_EQ(table.entities().size(), std::size(Entities) - std::size(Tombstones));
    for (auto entity : Entities) {
        if (entity != ECS::NullEntity) {
            ASSERT_EQ(static_cast<ECS::Entity>(*table.get(entity)), entity);
        }
    }
    table.traverse([&](const ECS::Entity entity) {
        ASSERT_NE(std::find(std::begin(Entities), std::end(Entities), entity), std::end(Entities));
    });
}

TEST(StableComponentTable, Random)
{
    static ECS::Entity LastEntity {};
    static int LastValue {};
    static std::vector<ECS::Entity> FreeEntities {};

    constexpr auto AddEntity = [] {
        if (FreeEntities.empty())
            return ++LastEntity;
        else {
            const auto e = FreeEntities.back();
            FreeEntities.pop_back();
            return e;
        }
    };

    constexpr auto RemoveEntity = [](const auto entity) {
        FreeEntities.push_back(entity);
    };

    struct Pair
    {
        ECS::Entity entity {};
        int value {};
    };

    std::vector<Pair> values {};

    ECS::StableComponentTable<int, 4096 / sizeof(ECS::Entity), 4096 / sizeof(int)> table;

    for (auto i = 0; i != 1000; ++i) {
        // Add
        if (table.entities().empty() || Core::Random::Generate32(3)) {
            const auto entity = AddEntity();
            const auto value = ++LastValue;
            table.add(entity, value);
            values.push_back(Pair { entity, value });
        // Remove
        } else {
            const auto index = Core::Random::Generate32(table.entities().size());
            const ECS::Entity entity = table.entities().at(index);
            if (entity == ECS::NullEntity)
                continue;
            const auto it = std::find_if(values.begin(), values.end(), [entity](const auto &pair) { return pair.entity == entity; });
            ASSERT_NE(it, values.end());
            ASSERT_EQ(it->value, table.get(entity));
            values.erase(it);
            table.remove(entity);
            RemoveEntity(entity);
        }

        if (!Core::Random::Generate32(4)) {
            table.sort([&](const auto &lhs, const auto &rhs) {
                return table.get(lhs) > table.get(rhs);
            });
        }
    }

    for (const auto &pair : values) {
        const auto it = table.entities().find(pair.entity);
        ASSERT_NE(it, table.entities().end());
        ASSERT_EQ(table.get(pair.entity), pair.value);
    }
}

struct Entry
{
    enum class Action { Insert, Remove };

    ECS::Entity entity;
    Action action;
    std::uint32_t sortCount {};
};

template<typename EntitySet>
static void TestEntries(const EntitySet &entitySet)
{
    StableComponentTableType table;

    const auto testTable = [&] {
        // Test all
        auto index = 0u;
        for (const auto entity : table.entities()) {
            if (entity == ECS::NullEntity) {
                ++index;
                continue;
            }
            const auto entityIndex = table.getUnstableIndex(entity);
            ASSERT_EQ(index, entityIndex);
            ASSERT_EQ(static_cast<ECS::Entity>(*table.get(entity)), entity);
            ASSERT_EQ(static_cast<ECS::Entity>(*table.atIndex(entityIndex)), entity);
            ++index;
        }
        table.traverse([&](const ECS::Entity entity) {
            ASSERT_EQ(static_cast<ECS::Entity>(*table.get(entity)), entity);
        });
        table.traverse([&](const ECS::Entity entity, auto &component) {
            ASSERT_EQ(static_cast<ECS::Entity>(*table.get(entity)), entity);
            ASSERT_EQ(entity, static_cast<ECS::Entity>(*component));
        });
    };
    const auto testInsert = [&](const ECS::Entity entity) {
        testTable();
        table.add(entity, std::make_unique<int>(static_cast<int>(entity)));
        testTable();
    };

    const auto testRemove = [&](const ECS::Entity entity) {
        testTable();
        table.remove(entity);
        testTable();
    };

    for (const Entry &entry : entitySet) {
        if (entry.action == Entry::Action::Insert)
            testInsert(entry.entity);
        else
            testRemove(entry.entity);
        if (entry.sortCount)
        for (auto i = 0u; i != entry.sortCount; ++i) {
            const auto ascent = [&](const auto &lhs, const auto &rhs) {
                return *table.get(lhs) < *table.get(rhs);
            };
            const auto descent = [&](const auto &lhs, const auto &rhs) {
                return *table.get(lhs) > *table.get(rhs);
            };
            if (i % 2)
                table.sort(descent);
            else
                table.sort(ascent);
        }
    }
}

TEST(StableComponentTable, Bug01)
{
    constexpr Entry EntitySet[] {
        { 5, Entry::Action::Insert }, { 13, Entry::Action::Insert }, { 15, Entry::Action::Insert }, { 17, Entry::Action::Insert },
        { 21, Entry::Action::Insert }, // <-- The one that glitched in real use-case
        { 29, Entry::Action::Insert }, { 31, Entry::Action::Insert }, { 33, Entry::Action::Insert }, { 37, Entry::Action::Insert },
        { 42, Entry::Action::Insert }, { 44, Entry::Action::Insert }, { 47, Entry::Action::Insert }, { 51, Entry::Action::Insert },
        { 53, Entry::Action::Insert }, { 56, Entry::Action::Insert }, { 59, Entry::Action::Insert }, { 63, Entry::Action::Insert },
        { 65, Entry::Action::Insert }, { 68, Entry::Action::Insert }, { 71, Entry::Action::Insert }, { 75, Entry::Action::Insert },
        { 77, Entry::Action::Insert }, { 80, Entry::Action::Insert, 2 }, { 85, Entry::Action::Insert }, { 88, Entry::Action::Insert },
        { 89, Entry::Action::Insert }, { 90, Entry::Action::Insert }, { 98, Entry::Action::Insert }, { 99, Entry::Action::Insert },
        { 100, Entry::Action::Insert }, { 101, Entry::Action::Insert }, { 102, Entry::Action::Insert }, { 114, Entry::Action::Insert },
        { 126, Entry::Action::Insert }, { 138, Entry::Action::Insert }, { 150, Entry::Action::Insert }, { 162, Entry::Action::Insert },
        { 174, Entry::Action::Insert }, { 186, Entry::Action::Insert }, { 198, Entry::Action::Insert }, { 210, Entry::Action::Insert },
        { 222, Entry::Action::Insert, 87 }, { 229, Entry::Action::Insert }, { 230, Entry::Action::Insert }, { 240, Entry::Action::Insert },
        { 242, Entry::Action::Insert }, { 243, Entry::Action::Insert }, { 244, Entry::Action::Insert }, { 237, Entry::Action::Insert },
        { 247, Entry::Action::Insert }, { 248, Entry::Action::Insert }, { 249, Entry::Action::Insert }, { 254, Entry::Action::Insert },
        { 255, Entry::Action::Insert }, { 256, Entry::Action::Insert }, { 257, Entry::Action::Insert }, { 258, Entry::Action::Insert },
        { 269, Entry::Action::Insert }, { 278, Entry::Action::Insert }, { 286, Entry::Action::Insert }, { 290, Entry::Action::Insert },
        { 293, Entry::Action::Insert }, { 278, Entry::Action::Remove }, { 278, Entry::Action::Insert }, { 297, Entry::Action::Insert },
        { 298, Entry::Action::Insert }, { 299, Entry::Action::Insert }, { 300, Entry::Action::Insert }, { 301, Entry::Action::Insert },
        { 302, Entry::Action::Insert }, { 303, Entry::Action::Insert }, { 304, Entry::Action::Insert, 25 }, { 5, Entry::Action::Remove },
        { 13, Entry::Action::Remove }, { 15, Entry::Action::Remove }, { 17, Entry::Action::Remove }, { 21, Entry::Action::Remove },
        { 29, Entry::Action::Remove }, { 31, Entry::Action::Remove }, { 33, Entry::Action::Remove }, { 37, Entry::Action::Remove },
        { 42, Entry::Action::Remove }, { 44, Entry::Action::Remove }, { 114, Entry::Action::Remove }, { 47, Entry::Action::Remove },
        { 126, Entry::Action::Remove }, { 51, Entry::Action::Remove }, { 53, Entry::Action::Remove }, { 138, Entry::Action::Remove },
        { 56, Entry::Action::Remove }, { 150, Entry::Action::Remove }, { 59, Entry::Action::Remove }, { 162, Entry::Action::Remove },
        { 63, Entry::Action::Remove }, { 65, Entry::Action::Remove }, { 174, Entry::Action::Remove }, { 68, Entry::Action::Remove },
        { 186, Entry::Action::Remove }, { 71, Entry::Action::Remove }, { 198, Entry::Action::Remove }, { 75, Entry::Action::Remove },
        { 77, Entry::Action::Remove }, { 210, Entry::Action::Remove }, { 80, Entry::Action::Remove }, { 222, Entry::Action::Remove },
        { 85, Entry::Action::Remove }, { 88, Entry::Action::Remove }, { 89, Entry::Action::Remove }, { 90, Entry::Action::Remove },
        { 98, Entry::Action::Remove }, { 99, Entry::Action::Remove }, { 100, Entry::Action::Remove }, { 101, Entry::Action::Remove },
        { 102, Entry::Action::Remove }, { 229, Entry::Action::Remove }, { 230, Entry::Action::Remove }, { 269, Entry::Action::Remove },
        { 278, Entry::Action::Remove }, { 297, Entry::Action::Remove }, { 298, Entry::Action::Remove }, { 299, Entry::Action::Remove },
        { 300, Entry::Action::Remove }, { 301, Entry::Action::Remove }, { 302, Entry::Action::Remove }, { 303, Entry::Action::Remove },
        { 304, Entry::Action::Remove }, { 237, Entry::Action::Remove }, { 240, Entry::Action::Remove }, { 242, Entry::Action::Remove },
        { 243, Entry::Action::Remove }, { 244, Entry::Action::Remove }, { 247, Entry::Action::Remove }, { 248, Entry::Action::Remove },
        { 249, Entry::Action::Remove }, { 254, Entry::Action::Remove }, { 255, Entry::Action::Remove }, { 256, Entry::Action::Remove },
        { 257, Entry::Action::Remove }, { 258, Entry::Action::Remove }, { 286, Entry::Action::Remove }, { 290, Entry::Action::Remove },
        { 293, Entry::Action::Remove }
    };

    TestEntries(EntitySet);
}

TEST(StableComponentTable, Bug02)
{
    constexpr Entry EntitySet[] {
        { 1, Entry::Action::Insert }, { 2, Entry::Action::Insert, 1 }, { 2, Entry::Action::Remove, 1 }, { 2, Entry::Action::Insert },
        { 3, Entry::Action::Insert, 1 }, { 3, Entry::Action::Remove, 1 }, { 2, Entry::Action::Remove }, { 1, Entry::Action::Remove },
        { 1, Entry::Action::Insert }, { 1, Entry::Action::Remove }, { 1, Entry::Action::Insert, 1 }, { 1, Entry::Action::Remove },
        { 1, Entry::Action::Insert }, { 1, Entry::Action::Remove }, { 1, Entry::Action::Insert }, { 2, Entry::Action::Insert },
        { 3, Entry::Action::Insert }, { 4, Entry::Action::Insert }, { 5, Entry::Action::Insert, 1 }, { 6, Entry::Action::Insert },
        { 7, Entry::Action::Insert }, { 8, Entry::Action::Insert, 1 }, { 7, Entry::Action::Remove }, { 8, Entry::Action::Remove },
        { 8, Entry::Action::Insert }, { 7, Entry::Action::Insert }, { 9, Entry::Action::Insert }, { 6, Entry::Action::Remove },
        { 6, Entry::Action::Insert }, { 3, Entry::Action::Remove }, { 3, Entry::Action::Insert, 1 }, { 1, Entry::Action::Remove },
        { 9, Entry::Action::Remove }, { 9, Entry::Action::Insert }, { 1, Entry::Action::Insert }, { 10, Entry::Action::Insert },
        { 11, Entry::Action::Insert }, { 12, Entry::Action::Insert }, { 13, Entry::Action::Insert }, { 13, Entry::Action::Remove, 1 },
        { 2, Entry::Action::Remove }, { 2, Entry::Action::Insert }, { 4, Entry::Action::Remove, 1 }, { 4, Entry::Action::Insert },
        { 11, Entry::Action::Remove }, { 2, Entry::Action::Remove }, { 2, Entry::Action::Insert, 1 }, { 7, Entry::Action::Remove, 1 },
        { 7, Entry::Action::Insert }, { 11, Entry::Action::Insert }, { 13, Entry::Action::Insert }, { 10, Entry::Action::Remove, 1 },
        { 10, Entry::Action::Insert }, { 6, Entry::Action::Remove }, { 9, Entry::Action::Remove }, { 9, Entry::Action::Insert },
        { 6, Entry::Action::Insert }, { 1, Entry::Action::Remove }, { 13, Entry::Action::Remove }, { 13, Entry::Action::Insert, 1 },
        { 1, Entry::Action::Insert, 1 }, { 6, Entry::Action::Remove, 1 }
    };

    TestEntries(EntitySet);
}