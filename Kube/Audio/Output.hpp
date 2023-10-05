/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio output device
 */

#pragma once

#include <Kube/Core/TrivialFunctor.hpp>

#include "Device.hpp"

namespace kF::Audio
{
    class Output;
}

/** @brief Physical output device abstraction */
class kF::Audio::Output final : public Device
{
public:
    /** @brief Audio callback functor */
    using Callback = Core::TrivialFunctor<void(float * const, const std::uint32_t, bool)>;


    /** @brief Destructor */
    ~Output(void) noexcept override = default;

    /** @brief Constructor */
    Output(const DeviceModel &deviceModel, const DeviceSpecs &deviceSpecs, Callback &&callback) noexcept
        : Device(deviceModel, deviceSpecs, false), _callback(std::move(callback)) {}


private:
    /** @brief Audio callback */
    void onAudioCallback(float * const data, const std::uint32_t channelSampleCount, const bool stereo) const noexcept final
        { _callback(data, channelSampleCount, stereo); }


    Callback _callback {};
};