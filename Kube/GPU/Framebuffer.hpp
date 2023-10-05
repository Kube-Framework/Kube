/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Framebuffer
 */

#pragma once

#include "Handle.hpp"
#include "FramebufferModel.hpp"

namespace kF::GPU
{
    class Framebuffer;
}

/** @brief Abstract a frame buffer */
class kF::GPU::Framebuffer : public Handle<FramebufferHandle>
{
public:
    /** @brief Initializer-list constructor */
    [[nodiscard]] static inline Framebuffer Make(const FramebufferCreateFlags flags, const RenderPassHandle renderPass,
            const std::initializer_list<ImageViewHandle> &attachments,
            const std::uint32_t width_, const std::uint32_t height_, const std::uint32_t layers_) noexcept
        { return Framebuffer(FramebufferModel(flags, renderPass, attachments.begin(), attachments.end(), width_, height_, layers_)); }


    /** @brief Destruct the pipelineLayout */
    ~Framebuffer(void) noexcept;

    /** @brief Default constructor */
    Framebuffer(void) noexcept = default;

    /** @brief Construct a pipeline using a model */
    Framebuffer(const FramebufferModel &model) noexcept;

    /** @brief Move constructor */
    Framebuffer(Framebuffer &&other) noexcept = default;

    /** @brief Move assignment */
    Framebuffer &operator=(Framebuffer &&other) noexcept = default;
};