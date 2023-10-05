/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Shader
 */

#include <fstream>

#include <Kube/Core/Abort.hpp>
#include <Kube/IO/File.hpp>

#include "GPU.hpp"
#include "Shader.hpp"

using namespace kF;

GPU::Shader::~Shader(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyShaderModule(parent().logicalDevice(), handle(), nullptr);
}

GPU::Shader::Shader(const std::string_view &path) noexcept
    : Shader(IO::File(path, IO::File::Mode::ReadBinary).readAll<Core::Vector<std::uint8_t, IO::IOAllocator>>(), path)
{
}

GPU::Shader::Shader(const BinaryCode &binary, const std::string_view &path) noexcept
{
    kFEnsure(!binary.empty(),
        "GPU::Shader: Empty binary code of shader '", path, '\'');
    const VkShaderModuleCreateInfo shaderModuleInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VkShaderModuleCreateFlags(),
        .codeSize = binary.size(),
        .pCode = reinterpret_cast<const std::uint32_t *>(binary.begin())
    };

    if (const auto res = ::vkCreateShaderModule(parent().logicalDevice(), &shaderModuleInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Shader: Couldn't create shader module '", ErrorMessage(res), "' at path '", path, '\'');
}