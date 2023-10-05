/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO ResourceManager
 */

#pragma once

#include <cmrc/cmrc.hpp>

#include <Kube/Core/Vector.hpp>
#include <Kube/Core/Hash.hpp>

#include "Base.hpp"

#define KF_DECLARE_RESOURCE_ENVIRONMENT(EnvironmentName) \
CMRC_DECLARE(EnvironmentName); \
namespace kF::IO::Modules \
{ \
    struct EnvironmentName \
    { \
        static inline const Environment Instance = [] { \
            const auto env = cmrc::EnvironmentName::get_filesystem(); \
            ResourceManager::RegisterEnvironmentLater(Core::Hash(#EnvironmentName), env); \
            return env; \
        }(); \
    }; \
} static_assert(true)

namespace kF::IO
{
    class ResourceManager;

    /** @brief Resource environment */
    using Environment = cmrc::embedded_filesystem;
}

/** @brief Manage all resource environments */
class alignas_half_cacheline kF::IO::ResourceManager
{
public:
    /** @brief Add environment to  */
    static void RegisterEnvironmentLater(
            const Core::HashedName environmentName, const Environment environment) noexcept;

    /** @brief Get manager global instance */
    [[nodiscard]] static inline ResourceManager &Get(void) noexcept { return *_Instance; }


    /** @brief Destructor */
    ~ResourceManager(void) noexcept;

    /** @brief Constructor */
    ResourceManager(void) noexcept;


    /** @brief Check if an environment exists */
    [[nodiscard]] bool environmentExists(const Core::HashedName environmentName) const noexcept;

    /** @brief Get a resource environment */
    [[nodiscard]] Environment getEnvironment(const Core::HashedName environment) const noexcept;


    /** @brief Check if a resource exists at 'path' inside 'environment' */
    [[nodiscard]] bool resourceExists(const Core::HashedName environmentName, const std::string_view &path) const noexcept;


    /** @brief Query a resource */
    [[nodiscard]] ResourceView queryResource(const Core::HashedName environmentName, const std::string_view &path) const noexcept;

private:
    /** @brief Register an environment into the manager */
    void registerEnvironment(const Core::HashedName environmentName, const Environment environment) noexcept;


    /** @brief Global instance */
    static ResourceManager *_Instance;

    Core::Vector<Core::HashedName, IOAllocator> _environmentNames {};
    Core::Vector<cmrc::embedded_filesystem, IOAllocator> _environments {};
};
static_assert_fit_half_cacheline(kF::IO::ResourceManager);