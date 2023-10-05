/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Shader
 */

#pragma once

#include <Kube/Core/Vector.hpp>

#include "Handle.hpp"

namespace kF::GPU
{
    class Shader;
}

/** @brief Abstraction of a GPU shader */
class kF::GPU::Shader : public Handle<VkShaderModule>
{
public:
    /** @brief A vector containing binary data */
    using BinaryCode = Core::IteratorRange<const std::uint8_t *>;


    /** @brief Construct a shader from file or resource system */
    Shader(const std::string_view &path) noexcept;

    /** @brief Construct a shader from binary code (path is used for error print) */
    Shader(const BinaryCode &binary, const std::string_view &path) noexcept;

    /** @brief Move constructor */
    Shader(Shader &&other) noexcept = default;

    /** @brief Destruct the shader */
    ~Shader(void) noexcept;

    /** @brief Move assignment */
    Shader &operator=(Shader &&other) noexcept = default;
};