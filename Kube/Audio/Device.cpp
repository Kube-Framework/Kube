/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio device
 */

#include <SDL2/SDL.h>

#include <Kube/Core/Abort.hpp>

#include "Device.hpp"

using namespace kF;

Audio::Device::~Device(void) noexcept
{
    SDL_CloseAudio();
    // SDL_CloseAudioDevice(_id);
}

Audio::Device::Device(const DeviceModel &deviceModel, const DeviceSpecs &deviceSpecs, const bool isCapture) noexcept
{
    kFEnsure(SDL_WasInit(SDL_INIT_AUDIO),
        "Audio::Device: Manager must be initialized before any device");

    SDL_AudioSpec desiredSpecs {
        .freq = static_cast<int>(deviceSpecs.sampleRate),
        .format = AUDIO_F32,
        .channels = static_cast<std::uint8_t>(deviceSpecs.stereo ? 2 : 1),
        .samples = static_cast<std::uint16_t>(deviceSpecs.blockSize),
        .callback = [](void * const device, std::uint8_t * const data, const int blockSize) {
            const auto audioDevice = reinterpret_cast<Device *>(device);
            audioDevice->onAudioCallback(
                reinterpret_cast<float *>(data),
                static_cast<std::uint32_t>(blockSize) / (static_cast<std::uint32_t>(sizeof(float)) * static_cast<std::uint32_t>(audioDevice->_stereo + 1)),
                audioDevice->_stereo
            );
        },
        .userdata = this
    };

    // Acquire device
    SDL_AudioSpec acquiredSpecs {};
    _id = SDL_OpenAudioDevice(deviceModel.data(), isCapture, &desiredSpecs, &acquiredSpecs, 0);
    if (_id <= 0)
        _id = SDL_OpenAudioDevice(nullptr, isCapture, &desiredSpecs, &acquiredSpecs, SDL_AUDIO_ALLOW_ANY_CHANGE);
    kFEnsure(_id > 0, "Audio::Manager::OpenDevice: Couldn't open audio device: '", SDL_GetError());

    // Update stereo
    _stereo = acquiredSpecs.channels == 2;

    kFInfo("[Audio] Device specs:\tFrequency ", acquiredSpecs.freq,
        " Format ", static_cast<std::int64_t>(acquiredSpecs.format),
        " Channels ", static_cast<std::int64_t>(acquiredSpecs.channels),
        " Silence ", static_cast<std::int64_t>(acquiredSpecs.silence),
        " Samples ", static_cast<std::int64_t>(acquiredSpecs.samples),
        " Byte size ", static_cast<std::int64_t>(acquiredSpecs.size)
    );
}

void Audio::Device::start(void) noexcept
{
    SDL_PauseAudioDevice(_id, 0);
    _started = true;
}

void Audio::Device::stop(void) noexcept
{
    SDL_PauseAudioDevice(_id, 1);
    _started = false;
}
