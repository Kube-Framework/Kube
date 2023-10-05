/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Scheduler
 */

#include <thread>

#include <gtest/gtest.h>

#include <Kube/Flow/Scheduler.hpp>
#include <Kube/Flow/Graph.hpp>

using namespace kF;

static std::size_t RepeatCount = 100;

static std::size_t MaxThreads = std::thread::hardware_concurrency();

#define ASSERT_WORKER_COUNT(scheduler, i) \
    ASSERT_EQ(scheduler.workerCount(), (i == 0 ? MaxThreads : i));

TEST(Scheduler, InitDestroy)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        ASSERT_WORKER_COUNT(scheduler, i);
    }
}

TEST(Scheduler, InitDestroyDelay)
{
    {
        Flow::Scheduler scheduler(2ul);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_WORKER_COUNT(scheduler, 2ul);
    }
    {
        Flow::Scheduler scheduler(MaxThreads);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_WORKER_COUNT(scheduler, MaxThreads);
    }
}

TEST(Scheduler, BasicTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        Flow::Graph graph;
        bool trigger = false;

        graph.add([&trigger] {
            trigger = true;
        });

        // Sleep for long tasks
        scheduler.schedule(graph);
        graph.waitSleep();
        ASSERT_EQ(trigger, true);

        // Spin in execution loop is way faster
        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = false;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, true);
        }
    }
}

TEST(Scheduler, AddRemoveTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        Flow::Graph graph;
        int trigger {};

        auto &task1 = graph.add([&trigger] {
            trigger = 1;
        });
        auto &task2 = graph.add([&trigger] {
            trigger = 2;
        });
        auto &task3 = graph.add([&trigger] {
            trigger = 3;
        });
        task1.before(task2);
        task1.before(task3);
        task2.before(task3);

        // Run graph
        scheduler.schedule(graph);
        graph.waitSleep();
        ASSERT_EQ(trigger, 3);

        // Remove task 2
        graph.remove(task2);
        scheduler.schedule(graph);
        graph.waitSleep();
        ASSERT_EQ(trigger, 3);

        // Remove task 3
        graph.remove(task3);
        scheduler.schedule(graph);
        graph.waitSleep();
        ASSERT_EQ(trigger, 1);
    }
}

TEST(Scheduler, SequenceTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        Flow::Graph graph;
        int trigger = 0;
        auto func = [&trigger] { ++trigger; };
        Flow::Task *last {};
        for (int j = 0; j < 100; ++j) {
            auto &tmp = graph.add(func);
            if (last)
                last->before(tmp);
            last = &tmp;
        }
        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = 0;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 100);
        }
    }
}

TEST(Scheduler, MergeTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        std::atomic<int> trigger = 0;
        Flow::Graph graph;
        auto &last = graph.add([&trigger]{
            trigger.store(100, std::memory_order_relaxed);
        });
        for (int j = 0; j < 100; ++j) {
            graph.add([&trigger, j] { trigger.store(j, std::memory_order_relaxed); })
                .before(last);
        }

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 100);
        }
    }
}

TEST(Scheduler, ConditionTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        std::atomic<int> trigger = 0;
        Flow::Graph graph;
        auto &a = graph.add([&trigger]() -> bool { return trigger % 2; }); // Implicit cast from bool to size_t
        auto &b = graph.add([&trigger] { trigger = 1; });
        auto &c = graph.add([&trigger] { trigger = 2; });
        a.before(b); // False returned
        a.before(c); // True returned

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = 0;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 1);

            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 2);

            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 1);
        }
    }
}

TEST(Scheduler, SwitchTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        int trigger = 0;
        int inside = 0;
        Flow::Graph graph;
        auto &a = graph.add([&trigger]() -> int { return trigger; }); // Implicit cast from int to size_t
        auto &b = graph.add([&inside, &trigger] { inside = 1; trigger = 1; });
        auto &c = graph.add([&inside, &trigger] { inside = 2; trigger = 2; });
        auto &d = graph.add([&inside, &trigger] { inside = 3; trigger = 3; });
        auto &e = graph.add([&inside, &trigger] { inside = 4; trigger = 4; });
        b.after(a); // 0 Returned
        c.after(a); // 1 Returned
        d.after(a); // 2 Returned
        e.after(a); // 3 Returned -> out of range, do not repeat

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = 0;
            // In range tests
            for (auto i = 1; i <= 4; ++i) {
                scheduler.schedule(graph);
                graph.waitSpin();
                ASSERT_EQ(trigger, i);
                ASSERT_EQ(inside, i);
            }
            // Border range test
            inside = 0;
            trigger = 4;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 4);
            ASSERT_EQ(inside, 0);
        }
    }
}

TEST(Scheduler, SwitchTaskAdvanced)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        std::atomic<int> trigger = 0;
        Flow::Graph graph;
        auto &a = graph.add([&trigger]() -> bool { return trigger != 0; }); // Implicit cast from bool to size_t
        auto &b = graph.add([&trigger] { trigger = 1; });
        auto &c = graph.add([&trigger] { trigger = 2; });
        auto &d = graph.add([&trigger] { trigger = 3; });
        auto &e = graph.add([&trigger] { trigger = 4; });
        auto &f = graph.add([&trigger] { trigger = 5; });
        b.after(a); // 0 returned
        c.after(a); // 1 returned
        d.after(c);
        e.after(c);
        f.after(d);
        f.after(e);

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = 0;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 1);
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 5);
        }
    }
}

TEST(Scheduler, SwitchMergePattern)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        std::int64_t condition = 0;
        int trigger = 0;
        Flow::Graph graph;
        auto &cond = graph.add([&condition]() -> std::int64_t { return condition; }); // Implicit cast from int64 to size_t
        auto &a = graph.add([&trigger] { trigger = 3; });
        auto &b = graph.add([&trigger] { trigger = 4; });
        auto &c = graph.add([&trigger] { trigger = 5; });
        auto &merge = graph.add([&trigger] { trigger *= 2; });
        cond.before(a); // 0 returned
        cond.before(b); // 1 returned
        cond.before(c); // 2 returned
        merge.after(a);
        merge.after(b);
        merge.after(c);

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            for (auto l = 0; l != 3; ++l) {
                trigger = 0;
                condition = l;
                scheduler.schedule(graph);
                graph.waitSpin();
                ASSERT_EQ(trigger, (3 + l) * 2);
            }
        }
    }
}

TEST(Scheduler, GraphTask)
{
    for (auto i = 0ul; i != MaxThreads; ++i) {
        Flow::Scheduler scheduler(i);
        int trigger = 0;
        auto func = [&trigger] {
            trigger += 1;
        };
        auto func2 = [&trigger] {
            trigger *= 2;
        };

        // Construct sub graph
        Flow::Graph subGraph;
        auto &subA = subGraph.add(func);
        auto &subB = subGraph.add(func2);
        subA.before(subB);

        // Construct graph A -> B -> C -> D
        Flow::Graph graph;
        auto &a = graph.add(func); // + 1 = 1
        auto &b = graph.add(&subGraph); // + 1 * 2 = 4
        auto &c = graph.add(&subGraph); // + 1 * 2 = 10
        auto &d = graph.add(func2); // * 2 = 20
        a.before(b);
        b.before(c);
        c.before(d);

        for (std::size_t k = 0; k < RepeatCount; ++k) {
            trigger = 0;
            scheduler.schedule(graph);
            graph.waitSpin();
            ASSERT_EQ(trigger, 20);
        }
    }
}
