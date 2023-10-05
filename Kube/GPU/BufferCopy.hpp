/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Buffer Copy
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct BufferCopy;
};

/** @brief Describe how to create a buffer copy */
struct kF::GPU::BufferCopy : public VkBufferCopy
{
    /** @brief Destructor */
    ~BufferCopy(void) noexcept = default;

    /** @brief Initialize constructor */
    BufferCopy(const BufferSize size_, const BufferSize srcOffset_ = 0u,
            const BufferSize dstOffset_ = 0u) noexcept
        : VkBufferCopy {
            .srcOffset = srcOffset_,
            .dstOffset = dstOffset_,
            .size = size_
        } {}

    /** @brief POD semantics */
    BufferCopy(const BufferCopy &other) noexcept = default;
    BufferCopy(BufferCopy &&other) noexcept = default;
    BufferCopy &operator=(const BufferCopy &other) noexcept = default;
    BufferCopy &operator=(BufferCopy &&other) noexcept = default;
};
