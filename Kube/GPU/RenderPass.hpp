/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RenderPass
 */

#pragma once

#include "Handle.hpp"
#include "RenderPassModel.hpp"

namespace kF::GPU
{
    class RenderPass;
}

/** @brief Abstraction of a render pass */
class kF::GPU::RenderPass : public Handle<RenderPassHandle>
{
public:
    /** @brief Make a RenderPass */
    [[nodiscard]] static inline RenderPass Make(
            const std::initializer_list<AttachmentDescription> &attachments,
            const std::initializer_list<SubpassDescription> &subpasses,
            const std::initializer_list<SubpassDependency> &dependencies) noexcept
        { return RenderPass(RenderPassModel(attachments.begin(), attachments.end(), subpasses.begin(), subpasses.end(), dependencies.begin(), dependencies.end())); }


    /** @brief Destruct the render pass */
    ~RenderPass(void) noexcept;

    /** @brief Construct a render pass */
    RenderPass(const RenderPassModel &model) noexcept;

    /** @brief Move constructor */
    RenderPass(RenderPass &&other) noexcept = default;

    /** @brief Move assignment */
    RenderPass &operator=(RenderPass &&other) noexcept = default;
};