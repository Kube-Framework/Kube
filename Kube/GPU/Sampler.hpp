/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sampler
 */

#pragma once

#include "Handle.hpp"
#include "SamplerModel.hpp"

namespace kF::GPU
{
    class Sampler;
};

class kF::GPU::Sampler : public Handle<SamplerHandle>
{
public:
    /** @brief Destruct the buffer */
    ~Sampler(void) noexcept;

    /** @brief Default constructor */
    Sampler(void) noexcept = default;

    /** @brief Construct the sampler using its model */
    Sampler(const SamplerModel &model) noexcept;

    /** @brief Move constructor */
    Sampler(Sampler &&other) noexcept = default;

    /** @brief Move assignment */
    Sampler &operator=(Sampler &&other) noexcept = default;
};