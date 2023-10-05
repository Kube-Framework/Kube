/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: LogicalDevice
 */

#pragma once

#include <Kube/Core/Vector.hpp>

#include "Handle.hpp"

namespace kF::GPU
{
    class LogicalDevice;
}

/** @brief The logical device abstracts the communication with a physical device */
class kF::GPU::LogicalDevice : public CachedHandle<VkDevice>
{
public:
    /** @brief Logical device extension list */
    using Extensions = Core::Vector<const char *>;

    /** @brief Describes the features of a device */
    struct DeviceFeatures
    {
        Extensions extensions {};
        PhysicalDeviceDescriptorIndexingFeatures indexingFeatures {};
        PhysicalDeviceFeatures2 features {};
    };

    /** @brief Unique pointer over DeviceFeatures to provide stable addressing */
    using DeviceFeaturesPtr = Core::UniquePtr<DeviceFeatures>;


    /** @brief Destruct the logical device*/
    ~LogicalDevice(void) noexcept;

    /** @brief Create a logical device */
    LogicalDevice(void) noexcept;

    /** @brief Move constructor */
    LogicalDevice(LogicalDevice &&other) noexcept = default;

    /** @brief Move assignment */
    LogicalDevice &operator=(LogicalDevice &&other) noexcept = default;


    /** @brief Wait until logical device is IDLE */
    void waitIdle(void) noexcept;

private:
    /** @brief Get device features */
    [[nodiscard]] DeviceFeaturesPtr getDeviceFeatures(void) const noexcept;

    /** @brief Get device extensions */
    [[nodiscard]] Extensions getExtensions(void) const noexcept;
};