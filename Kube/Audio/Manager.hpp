/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Audio manager
 */

#pragma once

#include <Kube/Core/UniquePtr.hpp>

#include "Input.hpp"
#include "Output.hpp"

/** @todo Specs inputs *différents* outputs ?! */

namespace kF::Audio
{
    class Manager;

    /** @brief Driver */
    using Driver = std::string_view;

    /** @brief List of audio drivers */
    using Drivers = Core::Vector<Driver, AudioAllocator>;
}

/** @brief Audio manager that loads drivers and devices */
class kF::Audio::Manager
{
public:
    /** @brief Destructor */
    ~Manager(void) noexcept;

    /** @brief Constructor */
    Manager(void) noexcept;


    /** @brief Get a list of available drivers */
    [[nodiscard]] Drivers getAvailableDrivers(void) const noexcept;

    /** @brief Get current driver */
    [[nodiscard]] inline const Driver &driver(void) noexcept;

    /** @brief Set current driver */
    void setDriver(const Driver &driver) noexcept;


    /** @brief Get a list of available input device models */
    [[nodiscard]] inline DeviceModels getAvailableInputModels(void) const noexcept
        { return getAvailableDeviceModels(true); }

    /** @brief Get a list of available output device models */
    [[nodiscard]] inline DeviceModels getAvailableOutputModels(void) const noexcept
        { return getAvailableDeviceModels(false); }

private:
    /** @brief Get a list of available input/output audio devices */
    [[nodiscard]] DeviceModels getAvailableDeviceModels(const bool isCapture) const noexcept;


    Driver _driver {};
};