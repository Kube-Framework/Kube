/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: RenderPassManager
 */

#pragma once

#include <Kube/Core/TrivialFunctor.hpp>
#include <Kube/Core/Vector.hpp>

#include "RenderPass.hpp"

namespace kF::GPU
{
    class RenderPassManager;

    /** @brief Functor that creates a render pass */
    using RenderPassFactory = Core::TrivialFunctor<RenderPass(void)>;
}

/** @brief A framebuffer handler holds a set of framebuffers */
class alignas_cacheline kF::GPU::RenderPassManager : public GPUObject
{
public:
    /** @brief Destoy the handler */
    ~RenderPassManager(void) noexcept = default;

    /** @brief Construct a render pass manager */
    RenderPassManager(const std::initializer_list<RenderPassFactory> &renderPassFactories) noexcept;

    /** @brief Move constructor */
    RenderPassManager(RenderPassManager &&other) noexcept = default;

    /** @brief Move assignment */
    RenderPassManager &operator=(RenderPassManager &&other) noexcept = default;


    /** @brief Return the number of render passes */
    [[nodiscard]] std::uint32_t renderPassCount(void) const noexcept { return _renderPasses.size(); }

    /** @brief Get render pass at index */
    [[nodiscard]] const RenderPass &renderPassAt(const std::uint32_t index) const noexcept { return _renderPasses.at(index); }


    /** @brief Callback on render view size changed */
    void onViewSizeChanged(void) noexcept;

private:
    Core::Vector<RenderPassFactory, GPUAllocator> _factories {};
    Core::Vector<RenderPass, GPUAllocator> _renderPasses {};
};

static_assert_fit_cacheline(kF::GPU::RenderPassManager);