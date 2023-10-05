/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of System
 */

#include <gtest/gtest.h>

#include <Kube/ECS/Executor.hpp>

using namespace kF;

using DummyPipeline = ECS::PipelineTag<"Dummy">;

class UselessSystem : public ECS::System<"Useless", DummyPipeline>
{
public:
    virtual bool tick(void) noexcept override { return false; }
};

TEST(System, Basics)
{
    ECS::Executor executor;
    executor.addPipeline<DummyPipeline>(60);
    auto &useless = executor.addSystem<UselessSystem>();

    ASSERT_EQ(&useless.parent(), &executor);
    ASSERT_EQ(useless.systemName(), "Useless");
    auto entity = useless.add();
    ASSERT_EQ(entity, 1);
    useless.remove(entity);
    entity = useless.add();
    ASSERT_EQ(entity, 1);
}

struct Foo
{
    int value {};
};
class FooSystem : public ECS::System<
    "Foo", DummyPipeline, Core::DefaultStaticAllocator,
    Foo
>
{
public:
};

TEST(System, SingleComponent)
{
    ECS::Executor executor;
    executor.addPipeline<DummyPipeline>(60);
    auto &foo = executor.addSystem<FooSystem>();

    // Add a single entity
    auto entity = foo.add(Foo { 42 });
    ASSERT_TRUE(foo.getTable<Foo>().exists(entity));
    ASSERT_EQ(foo.get<Foo>(entity).value, 42);

    // Add a range of entities
    const auto range = foo.addRange(10, Foo { 24 });

    // Check if all entities are inserted
    for (auto it = range.begin; it != range.end; ++it) {
        ASSERT_TRUE(foo.getTable<Foo>().exists(it));
        ASSERT_EQ(foo.get<Foo>(it).value, 24);
    }

    // Check if single entity is still preserved
    ASSERT_TRUE(foo.getTable<Foo>().exists(entity));
    ASSERT_EQ(foo.get<Foo>(entity).value, 42);


    // Remove the inserted range
    foo.removeRange(range);

    // Check if all entities are removed
    for (auto it = range.begin; it != range.end; ++it)
        ASSERT_FALSE(foo.getTable<Foo>().exists(it));

    // Check if single entity is still preserved
    ASSERT_TRUE(foo.getTable<Foo>().exists(entity));
    ASSERT_EQ(foo.get<Foo>(entity).value, 42);

    // Re-add the same range with different value
    const auto range2 = foo.addRange(10, Foo { 12 });
    ASSERT_EQ(range, range2);

    // Check if all entities are inserted
    for (auto it = range2.begin; it != range2.end; ++it) {
        ASSERT_TRUE(foo.getTable<Foo>().exists(it));
        ASSERT_EQ(foo.get<Foo>(it).value, 12);
    }

    // Check if single entity is still preserved
    ASSERT_TRUE(foo.getTable<Foo>().exists(entity));
    ASSERT_EQ(foo.get<Foo>(entity).value, 42);

    // Remove single entity
    foo.remove(entity);

    // Check if single entity is removed
    ASSERT_FALSE(foo.getTable<Foo>().exists(entity));

    // Re-add the same range with different value
    const auto range3 = foo.addRange(10, Foo { 8 });
    ASSERT_NE(range3, range);
    ASSERT_NE(range3, range2);
    ASSERT_EQ(range2.end, range3.begin);

    // Check if all entities are inserted
    for (auto it = range3.begin; it != range3.end; ++it) {
        ASSERT_TRUE(foo.getTable<Foo>().exists(it));
        ASSERT_EQ(foo.get<Foo>(it).value, 8);
    }

    // Remove a part of range2 & range3
    auto intersectRange = ECS::EntityRange { .begin = range2.begin + 5, .end = range2.end + 5 };
    foo.removeRange(intersectRange);

    // Check entities
    for (auto it = range2.begin; it != range3.end; ++it) {
        if (it < intersectRange.begin) {
            ASSERT_TRUE(foo.getTable<Foo>().exists(it));
            ASSERT_EQ(foo.get<Foo>(it).value, 12);
        } else if (it < intersectRange.end) {
            ASSERT_FALSE(foo.getTable<Foo>().exists(it));
        } else {
            ASSERT_TRUE(foo.getTable<Foo>().exists(it));
            ASSERT_EQ(foo.get<Foo>(it).value, 8);
        }
    }


    // Add a range that fits into removed entity range
    const auto range4 = foo.addRange(intersectRange.end - intersectRange.begin, Foo { 123 });
    ASSERT_EQ(range4, intersectRange);

    // Check entities
    for (auto it = range2.begin; it != range3.end; ++it) {
        if (it < intersectRange.begin) {
            ASSERT_TRUE(foo.getTable<Foo>().exists(it));
            ASSERT_EQ(foo.get<Foo>(it).value, 12);
        } else if (it < intersectRange.end) {
            ASSERT_TRUE(foo.getTable<Foo>().exists(it));
            ASSERT_EQ(foo.get<Foo>(it).value, 123);
        } else {
            ASSERT_TRUE(foo.getTable<Foo>().exists(it));
            ASSERT_EQ(foo.get<Foo>(it).value, 8);
        }
    }
}


struct BarA
{
    int value {};
};
struct BarB
{
    float value {};
};
class BarSystem : public ECS::System<
    "Bar", DummyPipeline, Core::DefaultStaticAllocator,
    BarA, BarB
>
{
public:
};

TEST(System, TwoComponents)
{
    ECS::Executor executor;
    executor.addPipeline<DummyPipeline>(60);
    auto &foo = executor.addSystem<BarSystem>();

    // Add entity with BarA
    auto entityA = foo.add(BarA { 42 });
    ASSERT_TRUE(foo.getTable<BarA>().exists(entityA));
    ASSERT_FALSE(foo.getTable<BarB>().exists(entityA));
    ASSERT_EQ(foo.get<BarA>(entityA).value, 42);

    // Add entity with BarB
    auto entityB = foo.add(BarB { 24.0f });
    ASSERT_FALSE(foo.getTable<BarA>().exists(entityB));
    ASSERT_TRUE(foo.getTable<BarB>().exists(entityB));
    ASSERT_EQ(foo.get<BarB>(entityB).value, 24.0f);

    // Add entity with BarA & BarB
    auto entityC = foo.add(BarA { 12 }, BarB { 21.0f });
    ASSERT_TRUE(foo.getTable<BarA>().exists(entityC));
    ASSERT_TRUE(foo.getTable<BarB>().exists(entityC));
    ASSERT_EQ(foo.get<BarA>(entityC).value, 12.0f);
    ASSERT_EQ(foo.get<BarB>(entityC).value, 21.0f);

    // Dettach BarA from entity A
    foo.dettach<BarA>(entityA);
    ASSERT_FALSE(foo.getTable<BarA>().exists(entityA));
    ASSERT_FALSE(foo.getTable<BarB>().exists(entityA));

    // Dettach BarB from entity B
    foo.dettach<BarB>(entityB);
    ASSERT_FALSE(foo.getTable<BarA>().exists(entityB));
    ASSERT_FALSE(foo.getTable<BarB>().exists(entityB));

    // Dettach BarB from entity C
    foo.dettach<BarB, BarA>(entityC);
    ASSERT_FALSE(foo.getTable<BarA>().exists(entityC));
    ASSERT_FALSE(foo.getTable<BarB>().exists(entityC));
}

class StableSystem : public ECS::System<
    "Bar", DummyPipeline, Core::DefaultStaticAllocator,
    ECS::StableComponent<BarB>
>
{
public:
};

TEST(System, StableComponent)
{
    ECS::Executor executor;
    executor.addPipeline<DummyPipeline>(60);
    auto &system = executor.addSystem<StableSystem>();

    system.pack<BarB>();
}