/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Instance
 */

#include <cstring>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Instance.hpp"

using namespace kF;

GPU::Instance::~Instance(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyInstance(handle(), nullptr);
}

GPU::Instance::Instance(const Core::Version applicationVersion) noexcept
    : _applicationVersion(applicationVersion)
{
    // Query extensions & layers
    auto extensions = getExtensions(parent().backendWindow());
    auto layers = getLayers();
#if KUBE_DEBUG_BUILD
    kFInfoRaw("[GPU] Extensions: ");
    bool first = true;
    for (const auto &extension : extensions) {
        if (!first)
            kFInfoRaw(", ");
        else
            first = false;
        kFInfoRaw(extension);
    }
    kFInfo();
    kFInfoRaw("[GPU] Layers: ");
    first = true;
    for (const auto &layer : layers) {
        if (!first)
            kFInfoRaw(", ");
        else
            first = false;
        kFInfoRaw(layer);
    }
    kFInfo();
#endif

    // Create instance
    VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = ::SDL_GetWindowTitle(parent().backendWindow()),
        .applicationVersion = static_cast<std::uint32_t>(VK_MAKE_VERSION(applicationVersion.major, applicationVersion.minor, applicationVersion.patch)),
        .pEngineName = "Kube",
        .engineVersion = static_cast<std::uint32_t>(VK_MAKE_VERSION(Core::KubeVersion.major, Core::KubeVersion.minor, Core::KubeVersion.patch)),
        .apiVersion = VK_API_VERSION_1_1
    };
    VkInstanceCreateInfo instanceInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = KUBE_PLATFORM_APPLE ? VkInstanceCreateFlags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR) : VkInstanceCreateFlags(),
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<std::uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    if (const auto res = ::vkCreateInstance(&instanceInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Instance:: Couldn't create instance '", ErrorMessage(res), '\'');
}

GPU::Instance::Layers GPU::Instance::getLayers(void) const noexcept
{
#if KUBE_DEBUG_BUILD
    Layers layers { "VK_LAYER_KHRONOS_validation" };
    std::uint32_t count = 0;
    Core::Vector<VkLayerProperties> avaible;

    if (const auto res = ::vkEnumerateInstanceLayerProperties(&count, nullptr); res != VK_SUCCESS)
        kFAbort("GPU::Instance::getLayers: Couldn't enumerate instances layers properties", ErrorMessage(res), '\'');
    if (!count)
        return layers;
    avaible.resize(count + layers.size());
    if (const auto res = ::vkEnumerateInstanceLayerProperties(&count, avaible.data()); res != VK_SUCCESS)
        kFAbort("GPU::Instance::getLayers: Couldn't enumerate instances layers properties", ErrorMessage(res), '\'');
    for (auto it = layers.begin(); it != layers.end();) {
        bool found = false;
        for (const auto &layerProperties : avaible) {
            if (found = !std::strcmp(*it, layerProperties.layerName); found)
                break;
        }
        if (found)
            ++it;
        else {
            kFError("[GPU] Couldn't use debug layer '", *it, '\'');
            it = layers.erase(it);
        }
    }
    return layers;
#else
    return Layers();
#endif
}

GPU::Instance::Extensions GPU::Instance::getExtensions(BackendWindow *window) const noexcept
{
    Extensions extensions;
    std::uint32_t count = 0;

    if (!::SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr))
        kFAbort("GPU::Instance::getExtensions: Couldn't retreive instance extensions count '", ::SDL_GetError(), '\'');
    if (!count)
        return extensions;
    extensions.resize(count);
    if (!::SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data()))
        kFAbort("GPU::Instance::getExtensions: Couldn't retreive instance extensions '", ::SDL_GetError(), '\'');
    if constexpr (KUBE_PLATFORM_APPLE) {
        extensions.push(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
    return extensions;
}