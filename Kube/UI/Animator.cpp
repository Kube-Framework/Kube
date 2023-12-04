/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: UI Animator
 */

#include <Kube/Core/Abort.hpp>

#include "Animator.hpp"

using namespace kF;

void UI::Animator::start(const Animation &animation) noexcept
{
    kFEnsure(animation.duration, "UI::Animator::start: Animation cannot have zero as duration");

    const auto index = findIndex(animation);
    AnimationState *state {};
    if (!index.success()) [[likely]] {
        state = &_states.push(AnimationState { .animation = &animation });
    } else {
        state = &_states.at(index.value());
        if (animation.statusEvent)
            animation.statusEvent(AnimationStatus::Stop);
    }
    state->elapsed = {};
    state->reverse = animation.reverse;
    ++state->startCount;
    if (animation.statusEvent)
        animation.statusEvent(AnimationStatus::Start);
}

void UI::Animator::stop(const Animation &animation) noexcept
{
    const auto index = findIndex(animation);

    if (!index.success())
        return;
    auto &state = _states.at(index.value());
    if (animation.statusEvent)
        animation.statusEvent(AnimationStatus::Stop);
    state.ended = true;
}

Core::Expected<std::uint32_t> UI::Animator::findIndex(const Animation &animation) const noexcept
{
    auto it = _states.find([&animation](const auto &state) { return &animation == state.animation; });

    if (it != _states.end()) [[likely]]
        return Core::Expected<std::uint32_t>(Core::Distance<std::uint32_t>(_states.begin(), it));
    else [[unlikely]]
        return Core::Expected<std::uint32_t>();
}

void UI::Animator::onTick(const std::int64_t elapsed) noexcept
{
    bool atLeastOneEnded {};
    for (auto index = 0u, count = _states.size(); index != count; ++index) {
        auto &state = _states[index];
        // Skip already ended animations
        if (state.ended) {
            atLeastOneEnded = true;
            continue;
        }
        // Process animation tick
        const auto &animation = *state.animation;
        const auto duration = std::max<std::int64_t>(animation.duration, 1);
        const auto totalElapsed = std::min(state.elapsed + elapsed, duration);
        if (animation.tickEvent) {
            const auto ratio = float(double(totalElapsed) / double(duration));
            const auto reversedRatio = state.reverse ? 1.0f - ratio : ratio;
            animation.tickEvent(reversedRatio);
        }
        // The animation is not completed yiet
        if (duration != totalElapsed) [[likely]] {
            state.elapsed = totalElapsed;
            continue;
        }
        // The animation is complete, we may have to bounce
        if (animation.animationMode == AnimationMode::Bounce)
            state.reverse = !state.reverse;
        // Process status event but watch if the animation has been manually restarted
        const auto oldStartCount = state.startCount;
        if (animation.statusEvent)
            animation.statusEvent(AnimationStatus::Finish);
        const bool manuallyRestarted = state.startCount != oldStartCount;
        state.elapsed = {};
        // If the animation
        if (animation.animationMode == AnimationMode::Single && !manuallyRestarted) {
            state.ended = true;
            atLeastOneEnded = true;
        }
    }

    // If at some animation ended, we have to remove them
    if (!atLeastOneEnded)
        return;
    const auto eraseIt = std::remove_if(_states.begin(), _states.end(), [this, elapsed](auto &state) { return state.ended; });
    if (eraseIt != _states.end())
        _states.erase(eraseIt, _states.end());
}