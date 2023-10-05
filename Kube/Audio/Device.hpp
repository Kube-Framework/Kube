/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio device
 */

#pragma once

#include <Kube/Core/Vector.hpp>

#include "Base.hpp"

namespace kF::Audio
{
    class Device;


    /** @brief Device device model */
    using DeviceModel = std::string_view;

    /** @brief List of audio device device models */
    using DeviceModels = Core::Vector<DeviceModel, AudioAllocator>;


    /** @brief Specifications of an audio device */
    struct DeviceSpecs
    {
        std::uint32_t sampleRate;
        std::uint32_t blockSize;
        bool stereo {};
    };
}

/** @brief Physical device abstraction */
class kF::Audio::Device
{
public:
    /** @brief Destructor */
    virtual ~Device(void) noexcept;

    /** @brief Constructor */
    Device(const DeviceModel &deviceModel, const DeviceSpecs &deviceSpecs, const bool isCapture) noexcept;


    /** @brief Check if device is started */
    [[nodiscard]] inline bool started(void) const noexcept { return _started; }

    /** @brief Start the device */
    void start(void) noexcept;

    /** @brief Stop the device */
    void stop(void) noexcept;


protected:
    /** @brief Virtual audio callback */
    virtual void onAudioCallback(float * const data, const std::uint32_t channelSampleCount, const bool stereo) const noexcept = 0;


    std::uint32_t _id {};
    bool _stereo {};
    bool _started {};
};
