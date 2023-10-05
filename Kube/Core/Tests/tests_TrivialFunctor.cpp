/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial functor unit tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/TrivialFunctor.hpp>

using namespace kF;

struct Foo
{
    int y { 2 };

    int memberFunction(const int x)
        { return x * y; }

    static int FreeFunction(const int x, const int y)
        { return x * y; }
};

TEST(TrivialFunctor, FreeBasics)
{
    Core::TrivialFunctor<int(int, int)> func;
    func.prepare<&Foo::FreeFunction>();
    ASSERT_TRUE(func);
    ASSERT_EQ(func(4, 2), 8);
    ASSERT_EQ(func(8, 2), 16);
    Core::TrivialFunctor<int(int, int)> func2(func);
    ASSERT_TRUE(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4, 2), 8);
    ASSERT_EQ(func2(8, 2), 16);
    func2 = std::move(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4, 2), 8);
    ASSERT_EQ(func2(8, 2), 16);
}

TEST(TrivialFunctor, MemberBasics)
{
    Foo foo;
    Core::TrivialFunctor<int(int)> func;
    func.prepare<&Foo::memberFunction>(&foo);
    ASSERT_TRUE(func);
    ASSERT_EQ(func(4), 8);
    ASSERT_EQ(func(8), 16);
    Core::TrivialFunctor<int(int)> func2(func);
    ASSERT_TRUE(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4), 8);
    ASSERT_EQ(func2(8), 16);
    func2 = std::move(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4), 8);
    ASSERT_EQ(func2(8), 16);
}

TEST(TrivialFunctor, TrivialClassFunctorBasics)
{
    int y = 1;
    Core::TrivialFunctor<int(int)> func([&y](const int x) {
        return x * y;
    });
    ++y;
    ASSERT_TRUE(func);
    ASSERT_EQ(func(4), 8);
    ASSERT_EQ(func(8), 16);
    Core::TrivialFunctor<int(int)> func2(func);
    ASSERT_TRUE(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4), 8);
    ASSERT_EQ(func2(8), 16);
    func2 = std::move(func);
    ASSERT_TRUE(func2);
    ASSERT_EQ(func2(4), 8);
    ASSERT_EQ(func2(8), 16);
}

TEST(TrivialFunctor, VariableArguments)
{
    ASSERT_EQ((Core::TrivialFunctor<int(int, int)>([] { return 3; })(1, 2)), 3);
    ASSERT_EQ((Core::TrivialFunctor<int(int, int)>([](int x) { return x; })(1, 2)), 1);
    ASSERT_EQ((Core::TrivialFunctor<int(int, int)>([](int, int y) { return y; })(1, 2)), 2);
    ASSERT_EQ((Core::TrivialFunctor<int(int, int)>([](int x, int y) { return x + y; })(1, 2)), 3);
}