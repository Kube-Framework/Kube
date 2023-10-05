/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PhysicalDevice
 */

#pragma once

#include <Kube/Core/Vector.hpp>
#include <Kube/Core/UniquePtr.hpp>

#include "Handle.hpp"

namespace kF::GPU
{
    class PhysicalDevice;
}

/** @brief A physical device represent a GPU card in the computer */
class kF::GPU::PhysicalDevice : public CachedHandle<PhysicalDeviceHandle>
{
public:
    /** @brief A list of vulkan devices */
    using Devices = Core::Vector<PhysicalDeviceHandle>;


    /** @brief Destruct the physical device */
    ~PhysicalDevice(void) noexcept = default;

    /** @brief Construct a physical device */
    PhysicalDevice(void) noexcept;

    /** @brief Move constructor */
    PhysicalDevice(PhysicalDevice &&other) noexcept = default;

    /** @brief Move assignment */
    PhysicalDevice &operator=(PhysicalDevice &&other) noexcept = default;


    /** @brief Get physical device properties */
    [[nodiscard]] const PhysicalDeviceProperties &properties(void) const noexcept { return *_properties; }

    /** @brief Get physical device name */
    [[nodiscard]] std::string_view name(void) const noexcept { return _properties->deviceName; }

    /** @brief Get physical device limits */
    [[nodiscard]] const PhysicalDeviceLimits &limits(void) const noexcept { return _properties->limits; }


    /** @brief Find a supported format from candidate list */
    [[nodiscard]] Format findSupportedFormat(const ImageTiling tiling, const FormatFeatureFlags features,
            const std::initializer_list<Format> &candidates) noexcept
        { return findSupportedFormat(tiling, features, candidates.begin(), candidates.end()); }


    /** @brief Find a supported format from candidate list */
    [[nodiscard]] Format findSupportedFormat(const ImageTiling tiling, const FormatFeatureFlags features,
            const Format * const candidateBegin, const Format * const candidateEnd) const noexcept;


private:
    Core::UniquePtr<VkPhysicalDeviceProperties, GPUAllocator> _properties {};

    /** @brief Get the list of physical devices available */
    [[nodiscard]] Devices getDevices(void) const noexcept;

    /** @brief Select the best physical device to use */
    void selectDevice(const Devices &devices) noexcept;
};