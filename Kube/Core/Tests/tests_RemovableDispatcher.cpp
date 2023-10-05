/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial functor unit tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/RemovableTrivialDispatcher.hpp>
#include <Kube/Core/RemovableDispatcher.hpp>

using namespace kF;

struct Foo
{
    int memberFunction(const int x, const int y)
    { return x * y; }

    [[nodiscard]] static int FreeFunction(const int x, const int y)
        { return x * y; }
};

TEST(RemovableTrivialDispatcher, Basics)
{
    Core::RemovableTrivialDispatcher<int(int, int)> dispatcher;
    Foo foo;

    auto handle1 = dispatcher.add<&Foo::memberFunction>(&foo);
    auto handle2 = dispatcher.add<&Foo::FreeFunction>();
    auto handle3 = dispatcher.add([](int x, int y) {
        return x * y;
    });
    ASSERT_EQ(dispatcher.count(), 3);
    auto i = 0u;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 3);
    handle1 = {};
    handle2 = {};
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 1);
    handle1 = dispatcher.add<&Foo::FreeFunction>();
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 2);
    handle1 = {};
    handle2 = {};
    handle3 = {};
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 0);
}


TEST(RemovableTrivialDispatcher, Semantics)
{
    Core::RemovableTrivialDispatcher<int(int, int)> dispatcher;
    Foo foo;

    auto handle1 = dispatcher.add<&Foo::memberFunction>(&foo);
    auto handle2 = dispatcher.add<&Foo::FreeFunction>();
    auto handle3 = dispatcher.add([](int x, int y) {
        return x * y;
    });

    ASSERT_EQ(dispatcher.count(), 3);
    auto i = 0u;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 3);
    auto dispatcher2 = std::move(dispatcher);
    handle2 = {};
    i = 0;
    dispatcher2.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 2);
}


TEST(RemovableDispatcher, Basics)
{
    Core::RemovableDispatcher<int(int, int)> dispatcher;
    Foo foo;

    auto handle1 = dispatcher.add<&Foo::memberFunction>(&foo);
    auto handle2 = dispatcher.add<&Foo::FreeFunction>();
    auto handle3 = dispatcher.add([](int x, int y) {
        return x * y;
    });
    ASSERT_EQ(dispatcher.count(), 3);
    auto i = 0u;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 3);
    handle1 = {};
    handle2 = {};
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 1);
    handle1 = dispatcher.add<&Foo::FreeFunction>();
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 2);
    handle1 = {};
    handle2 = {};
    handle3 = {};
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 0);
}


TEST(RemovableDispatcher, Semantics)
{
    Core::RemovableDispatcher<int(int, int)> dispatcher;
    Foo foo;

    auto handle1 = dispatcher.add<&Foo::memberFunction>(&foo);
    auto handle2 = dispatcher.add<&Foo::FreeFunction>();
    auto handle3 = dispatcher.add([](int x, int y) {
        return x * y;
    });

    ASSERT_EQ(dispatcher.count(), 3);
    auto i = 0u;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 3);
    auto dispatcher2 = std::move(dispatcher);
    handle2 = {};
    i = 0;
    dispatcher2.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 2);
}
