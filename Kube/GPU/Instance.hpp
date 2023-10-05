/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Instance
 */

#pragma once

#include "Handle.hpp"

namespace kF::GPU
{
    class Instance;
}

/** @brief Abstraction of the low-level API instance */
class kF::GPU::Instance : public CachedHandle<VkInstance>
{
public:
    /** @brief Instance extension list */
    using Extensions = Core::Vector<const char *>;

    /** @brief Debug layers list */
    using Layers = Core::Vector<const char *>;


    /** @brief Destroy the instance */
    ~Instance(void) noexcept;

    /** @brief Construct an instance */
    Instance(const Core::Version applicationVersion) noexcept;

    /** @brief Move constructor */
    Instance(Instance &&other) noexcept = default;

    /** @brief Move assignment */
    Instance &operator=(Instance &&other) noexcept = default;


    /** @brief Get application version */
    [[nodiscard]] inline Core::Version applicationVersion(void) const noexcept { return _applicationVersion; }


private:
    /** @brief Get the list of debug layers */
    [[nodiscard]] Layers getLayers(void) const noexcept;

    /** @brief Get the list of instance extensions */
    [[nodiscard]] Extensions getExtensions(BackendWindow *window) const noexcept;

    Core::Version _applicationVersion;
};
