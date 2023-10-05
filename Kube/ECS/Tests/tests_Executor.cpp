/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Executor
 */

#include <thread>
#include <cmath>

#include <gtest/gtest.h>

#include <Kube/ECS/Executor.hpp>


#define STRINGIFY(x) #x

using namespace kF;
using namespace kF::Core::Literal;

template<typename Callback>
static void SetupExponentialWorkImpl(Flow::Graph &graph, Flow::Task &parent,
        const std::size_t iteration, const std::size_t maxIteration, Callback &&callback) noexcept
{
    for (auto i = 0ul; i < iteration; ++i) {
        auto &node = graph.add(callback);
        node.after(parent);
        if (iteration * 2 != maxIteration)
            SetupExponentialWorkImpl(graph, node, iteration * 2, maxIteration, callback);
    }
}

template<typename SystemType, typename Callback>
static void SetupExponentialWork(SystemType &system, const std::size_t depth, Callback &&callback) noexcept
{
    kFEnsure(depth > 0, "Depth must not be zero");

    auto &graph = system.taskGraph();
    auto &root = system.taskGraph().add(callback);

    if (depth > 1)
        SetupExponentialWorkImpl(graph, root, 2, static_cast<std::size_t>(std::pow(2ul, depth)), std::forward<Callback>(callback));
}

using SamplePipeline = ECS::PipelineTag<"Sample"_fixed>;

// System that samples its tickrate
template<auto Literal = "Sample"_fixed, typename TargetPipeline = SamplePipeline>
class SampleSystem : public ECS::System<Literal, TargetPipeline>
{
public:
    SampleSystem(std::atomic_size_t *counter) : _counter(counter) {}

    [[nodiscard]] bool tick(void) noexcept override
    {
        const std::size_t count = *_counter;
        if (count != 0) {
            _counts.push(count);
            *_counter = 0;
        }
        // Record current time and do not schedule graph
        _samples.push(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        return true;
    }

    /** @brief Execute samples test */
    void test(const double errorMargin = 0.05) noexcept
    {
        // Assert work count
        const auto taskCount = this->taskGraph().count();
        _counts.push(*_counter);
        for (const auto count : _counts)
            ASSERT_EQ(count, taskCount);

        // Assert number of samples
        ASSERT_GE(_samples.size(), 2);

        // Compute average tick rate
        auto it = _samples.begin();
        const auto end = _samples.end();
        auto last = *it;
        double average = 0;
        const auto sampleCount = static_cast<double>(_samples.size() - 1);
        while (++it != end) {
            average += static_cast<double>(*it - last) / sampleCount;
            last = *it;
        }

        // Ensure average tick rate fit within error margin
        const auto rate = static_cast<double>(this->tickRate());
        const auto diff = std::abs(rate - average);
        // kFInfo("AVERAGE ", average, ", ", diff, ", ", rate, ", ", errorMargin);
        ASSERT_LE(diff, rate * errorMargin);
    }

private:
    Core::Vector<std::int64_t> _samples {};
    Core::Vector<std::size_t> _counts {};
    std::atomic_size_t *_counter {};
};

// Generate a single system test that must tick at a given frequency
#define TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, Hertz) \
TEST(Executor, SampleTiming##WorkName##Hertz##Hz) \
{ \
    ECS::Executor executor; \
    std::atomic_size_t workCount {}; \
    executor.addPipeline<SamplePipeline, ECS::PipelineTimeMode::Free>(Hertz); \
    auto &sampleSystem = executor.addSystem<SampleSystem<>>(&workCount); \
    Setup##WorkName(sampleSystem, [&workCount] { ++workCount; }); \
    std::thread thd([&executor] { \
        std::this_thread::sleep_for(std::chrono::nanoseconds(std::max(ECS::HzToRate(Hertz) * 4, ECS::HzToRate(2)))); \
        executor.stop(); \
    }); \
    executor.run(); \
    sampleSystem.test(); \
    if (thd.joinable()) \
        thd.join(); \
}

#define GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(WorkName) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 4) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 24) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 30) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 60) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 75) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 120) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 166) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 240) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 480) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 1920) \
    TEST_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING(WorkName, 10000) \

// Work types
template<typename SystemType, typename Callback>
static void SetupNoWork(SystemType &, Callback &&) noexcept {}
template<typename SystemType, typename Callback>
static void SetupNegligableWork(SystemType &system, Callback &&callback) noexcept { SetupExponentialWork(system, 1, std::forward<Callback>(callback)); }
template<typename SystemType, typename Callback>
static void SetupLightWork(SystemType &system, Callback &&callback) noexcept      { SetupExponentialWork(system, 2, std::forward<Callback>(callback)); }
template<typename SystemType, typename Callback>
static void SetupMediumWork(SystemType &system, Callback &&callback) noexcept     { SetupExponentialWork(system, 3, std::forward<Callback>(callback)); }
template<typename SystemType, typename Callback>
static void SetupHeavyWork(SystemType &system, Callback &&callback) noexcept      { SetupExponentialWork(system, 4, std::forward<Callback>(callback)); }
template<typename SystemType, typename Callback>
static void SetupHardcoreWork(SystemType &system, Callback &&callback) noexcept   { SetupExponentialWork(system, 5, std::forward<Callback>(callback)); }


TEST(Executor, GUIScenario)
{
    // Event pipeline
    using EventPipeline = ECS::PipelineTag<"Event"_fixed>;
    using EventSystem = SampleSystem<"Event"_fixed, EventPipeline>;

    // Graphic pipeline
    using GraphicPipeline = ECS::PipelineTag<"Graphic"_fixed>;
    using UISystem = SampleSystem<"UI"_fixed, GraphicPipeline>;
    using PresentSystem = SampleSystem<"Present"_fixed, GraphicPipeline>;

    // Audio pipeline
    using AudioPipeline = ECS::PipelineTag<"Audio"_fixed>;
    using AudioSystem = SampleSystem<"Audio"_fixed, AudioPipeline>;

    ECS::Executor executor;

    executor.addPipeline<EventPipeline>(120ll);
    executor.addPipeline<GraphicPipeline>(60ll);
    executor.addPipeline<AudioPipeline, ECS::PipelineTimeMode::Bound>(75ll);


    // Event: Event system
    std::atomic_size_t eventWorkCount {};
    auto &eventSystem = executor.addSystem<EventSystem>(&eventWorkCount);
    SetupNegligableWork(eventSystem, [&eventWorkCount] { ++eventWorkCount; });


    // Graphic: Present system
    std::atomic_size_t presentWorkCount {};
    auto &presentSystem = executor.addSystem<PresentSystem>(&presentWorkCount);
    SetupLightWork(presentSystem, [&presentWorkCount] { ++presentWorkCount; });

    // Graphic: UI system (depends on PresentSystem)
    std::atomic_size_t uiWorkCount {};
    auto &uiSystem = executor.addSystem<UISystem, ECS::RunBefore<PresentSystem>>(&uiWorkCount);
    SetupHeavyWork(uiSystem, [&uiWorkCount] { ++uiWorkCount; });


    // Audio: Audio system
    std::atomic_size_t audioWorkCount {};
    auto &audioSystem = executor.addSystem<AudioSystem>(&audioWorkCount);
    SetupHardcoreWork(audioSystem, [&audioWorkCount] { ++audioWorkCount; });


    std::thread thd([&executor] {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        executor.stop();
    });

    executor.run();

    // Test systems
    eventSystem.test();
    presentSystem.test();
    uiSystem.test();
    audioSystem.test();

    if (thd.joinable())
        thd.join();
}

TEST(Executor, GameScenario)
{
    // Event pipeline
    using EventPipeline = ECS::PipelineTag<"Event"_fixed>;
    using EventSystem = SampleSystem<"Event"_fixed, EventPipeline>;

    // Graphic pipeline
    using GraphicPipeline = ECS::PipelineTag<"Graphic"_fixed>;
    using UISystem = SampleSystem<"UI"_fixed, GraphicPipeline>;
    using WorldSystem = SampleSystem<"World"_fixed, GraphicPipeline>;
    using PresentSystem = SampleSystem<"Present"_fixed, GraphicPipeline>;

    // Update pipeline
    using UpdatePipeline = ECS::PipelineTag<"Update"_fixed>;
    using GameplaySystem = SampleSystem<"Gameplay"_fixed, UpdatePipeline>;
    using PhysicSystem = SampleSystem<"Physic"_fixed, UpdatePipeline>;

    // Audio pipeline
    using AudioPipeline = ECS::PipelineTag<"Audio"_fixed>;
    using AudioSystem = SampleSystem<"Audio"_fixed, AudioPipeline>;

    ECS::Executor executor;

    executor.addPipeline<EventPipeline>(120ll);
    executor.addPipeline<GraphicPipeline>(60ll);
    executor.addPipeline<UpdatePipeline>(100ll);
    executor.addPipeline<AudioPipeline, ECS::PipelineTimeMode::Bound>(75ll);

    // Event: Event system
    std::atomic_size_t eventWorkCount {};
    auto &eventSystem = executor.addSystem<EventSystem>(&eventWorkCount);
    SetupNegligableWork(eventSystem, [&eventWorkCount] { ++eventWorkCount; });


    // Graphic: Present system
    std::atomic_size_t presentWorkCount {};
    auto &presentSystem = executor.addSystem<PresentSystem>(&presentWorkCount);
    SetupLightWork(presentSystem, [&presentWorkCount] { ++presentWorkCount; });

    // Graphic: UI system (depends on PresentSystem)
    std::atomic_size_t uiWorkCount {};
    auto &uiSystem = executor.addSystem<UISystem, ECS::RunBefore<PresentSystem>>(&uiWorkCount);
    SetupMediumWork(uiSystem, [&uiWorkCount] { ++uiWorkCount; });

    // Graphic: World system (depends on PresentSystem)
    std::atomic_size_t worldWorkCount {};
    auto &worldSystem = executor.addSystem<WorldSystem, ECS::RunBefore<PresentSystem>>(&worldWorkCount);
    SetupMediumWork(worldSystem, [&worldWorkCount] { ++worldWorkCount; });


    // Update: Physics system (depends on PresentSystem)
    std::atomic_size_t physicWorkCount {};
    auto &physicSystem = executor.addSystem<PhysicSystem>(&physicWorkCount);
    SetupHardcoreWork(physicSystem, [&physicWorkCount] { ++physicWorkCount; });

    // Update: Gameplay system
    std::atomic_size_t gameplayWorkCount {};
    auto &gameplaySystem = executor.addSystem<GameplaySystem, ECS::RunAfter<PhysicSystem>>(&gameplayWorkCount);
    SetupMediumWork(gameplaySystem, [&gameplayWorkCount] { ++gameplayWorkCount; });


    // Audio: Audio system
    std::atomic_size_t audioWorkCount {};
    auto &audioSystem = executor.addSystem<AudioSystem>(&audioWorkCount);
    SetupHeavyWork(audioSystem, [&audioWorkCount] { ++audioWorkCount; });


    std::thread thd([&executor] {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        executor.stop();
    });

    executor.run();

    // Test systems
    eventSystem.test();
    presentSystem.test();
    uiSystem.test();
    worldSystem.test();
    physicSystem.test();
    gameplaySystem.test();
    audioSystem.test();

    if (thd.joinable())
        thd.join();
}


// Generate individual tests of each work type
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(NoWork)
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(NegligableWork)
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(LightWork)
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(MediumWork)
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(HeavyWork)
GENERATE_EXECUTOR_INDIVIDUAL_SAMPLE_TIMING_RANGE(HardcoreWork)