/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio manager
 */

#include <SDL2/SDL.h>

#include <Kube/Core/Abort.hpp>

#include "Manager.hpp"

using namespace kF;

Audio::Manager::~Manager(void) noexcept
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

Audio::Manager::Manager(void) noexcept
    : _driver([] {
        kFEnsure(!SDL_WasInit(SDL_INIT_AUDIO),
            "Audio::Manager: Manager already initialized");

        kFEnsure(!SDL_Init(SDL_INIT_AUDIO),
            "Audio::Manager: Couldn't initialize SDL2 audio subsystem (", SDL_GetError(), ')');

        kFEnsure(SDL_WasInit(SDL_INIT_AUDIO),
            "Audio::Manager: Audio initialization failed");

        Driver driver(SDL_GetCurrentAudioDriver());
        kFInfo("[Audio] Driver loaded '", driver, '\'');
        return driver;
    }())
{
}

Audio::Drivers Audio::Manager::getAvailableDrivers(void) const noexcept
{
    const auto driverCount = SDL_GetNumAudioDrivers();
    Drivers drivers(static_cast<std::uint32_t>(driverCount));

    for (auto i = 0; i != driverCount; ++i)
        drivers[static_cast<std::uint32_t>(i)] = SDL_GetAudioDriver(i);
    return drivers;
}

void Audio::Manager::setDriver(const Driver &driver) noexcept
{
    _driver = driver;
    SDL_AudioQuit();
    kFEnsure(!SDL_AudioInit(driver.data()),
        "Audio::Manager: Couldn't initialize driver '", driver, "' (", SDL_GetError(), ')');
    kFInfo("[Audio] Driver loaded '", driver, '\'');
}

Audio::DeviceModels Audio::Manager::getAvailableDeviceModels(const bool isCapture) const noexcept
{
    const auto deviceCount = SDL_GetNumAudioDevices(isCapture);
    DeviceModels deviceModels(static_cast<std::uint32_t>(deviceCount));

    for (auto i = 0; i != deviceCount; ++i)
        deviceModels[static_cast<std::uint32_t>(i)] = SDL_GetAudioDeviceName(i, isCapture);
    return deviceModels;
}