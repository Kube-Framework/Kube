/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Pipeline
 */

#pragma once

#include "Handle.hpp"
#include "GraphicPipelineModel.hpp"
#include "ComputePipelineModel.hpp"

namespace kF::GPU
{
    class Pipeline;
}

/** @brief Abstraction of a GPU pipeline */
class kF::GPU::Pipeline : public Handle<PipelineHandle>
{
public:
    /** @brief Destruct the pipeline */
    ~Pipeline(void) noexcept;

    /** @brief Construct a pipeline using a graphic model */
    Pipeline(const GraphicPipelineModel &model) noexcept;

    /** @brief Construct a pipeline using a compute model */
    Pipeline(const ComputePipelineModel &model) noexcept;

    /** @brief Move constructor */
    Pipeline(Pipeline &&other) noexcept = default;

    /** @brief Move assignment */
    Pipeline &operator=(Pipeline &&other) noexcept = default;
};