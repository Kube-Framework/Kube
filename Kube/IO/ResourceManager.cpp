/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: IO ResourceManager
 */

#include <Kube/Core/TrivialDispatcher.hpp>
#include <Kube/Core/Abort.hpp>

#include "ResourceManager.hpp"

using namespace kF;

IO::ResourceManager *IO::ResourceManager::_Instance {};

static Core::TrivialDispatcher<
    void(void), Core::DefaultStaticAllocator, Core::CacheLineEighthSize * 4
> RegisterDispatcher {};

void IO::ResourceManager::RegisterEnvironmentLater(
        const Core::HashedName environmentName, const Environment environment) noexcept
{
    RegisterDispatcher.add([environmentName, environment]{
        ResourceManager::Get().registerEnvironment(environmentName, environment);
    });
}

IO::ResourceManager::~ResourceManager(void) noexcept
{
    kFEnsure(_Instance != nullptr,
        "IO::ResourceManager: ResourceManager is already destroyed");
    _Instance = nullptr;
}

IO::ResourceManager::ResourceManager(void) noexcept
{
    kFEnsure(_Instance == nullptr,
        "IO::ResourceManager: ResourceManager is already initialized");
    _Instance = this;
    RegisterDispatcher.dispatch();
}

void IO::ResourceManager::registerEnvironment(const Core::HashedName environmentName, const Environment environment) noexcept
{
    kFEnsure(!environmentExists(environmentName),
        "IO::ResourceManager: Environment already registered");
    _environmentNames.push(environmentName);
    _environments.push(environment);
}

bool IO::ResourceManager::environmentExists(const Core::HashedName environmentName) const noexcept
{
    return _environmentNames.find(environmentName) != _environmentNames.end();
}

IO::Environment IO::ResourceManager::getEnvironment(const Core::HashedName environmentName) const noexcept
{
    const auto it = _environmentNames.find(environmentName);
    kFEnsure(it != _environmentNames.end(),
        "IO::ResourceManager::getEnvironment: Environment is not registered");
    const auto index = Core::Distance<std::uint32_t>(_environmentNames.begin(), it);
    return _environments.at(index);
}

bool IO::ResourceManager::resourceExists(const Core::HashedName environmentName, const std::string_view &path) const noexcept
{
    return getEnvironment(environmentName).exists(path);
}

IO::ResourceView IO::ResourceManager::queryResource(
    const Core::HashedName environmentName, const std::string_view &path) const noexcept
{
    const auto file = getEnvironment(environmentName).open(path);

    return ResourceView {
        .from = reinterpret_cast<const std::uint8_t *>(file.begin()),
        .to = reinterpret_cast<const std::uint8_t *>(file.end())
    };
}