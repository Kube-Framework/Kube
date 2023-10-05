/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PipelineLayout
 */

#pragma once

#include "Handle.hpp"
#include "PipelineLayoutModel.hpp"

namespace kF::GPU
{
    class PipelineLayout;
}

/** @brief Abstract a pipeline layout */
class kF::GPU::PipelineLayout : public Handle<PipelineLayoutHandle>
{
public:
    /** @brief Initializer list constructor */
    [[nodiscard]] static inline PipelineLayout Make(
            const std::initializer_list<DescriptorSetLayoutHandle> &setLayouts,
            const std::initializer_list<PushConstantRange> &pushConstants
        ) noexcept
        { return PipelineLayout(PipelineLayoutModel(setLayouts.begin(), setLayouts.end(), pushConstants.begin(), pushConstants.end())); }

    /** @brief Initializer list constructor (DescriptorSetLayoutHandle only) */
    [[nodiscard]] static inline PipelineLayout Make(const std::initializer_list<DescriptorSetLayoutHandle> &setLayouts) noexcept
        { return PipelineLayout(PipelineLayoutModel(setLayouts.begin(), setLayouts.end(), nullptr, nullptr)); }

    /** @brief Initializer list constructor (PushConstantRange only) */
    [[nodiscard]] static inline PipelineLayout Make(const std::initializer_list<PushConstantRange> &pushConstants) noexcept
        { return PipelineLayout(PipelineLayoutModel(nullptr, nullptr, pushConstants.begin(), pushConstants.end())); }


    /** @brief Destruct the pipelineLayout */
    ~PipelineLayout(void) noexcept;

    /** @brief Default constructor */
    PipelineLayout(void) noexcept = default;

    /** @brief Construct a pipeline using a model */
    PipelineLayout(const PipelineLayoutModel &model) noexcept;

    /** @brief Move constructor */
    PipelineLayout(PipelineLayout &&other) noexcept = default;

    /** @brief Move assignment */
    PipelineLayout &operator=(PipelineLayout &&other) noexcept = default;
};