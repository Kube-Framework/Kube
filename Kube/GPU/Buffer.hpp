/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Buffer
 */

#pragma once

#include "Handle.hpp"
#include "BufferModel.hpp"

namespace kF::GPU
{
    class Buffer;
}

/** @brief GPU Memory buffer abstraction */
class kF::GPU::Buffer : public Handle<BufferHandle>
{
public:
    /** @brief Make an exclusive buffer */
    [[nodiscard]] static inline Buffer MakeExclusive(const BufferSize size, const BufferUsageFlags customUsage) noexcept
        { return Buffer(BufferModel::MakeExclusive(size, customUsage)); }

    /** @brief Make a staging buffer */
    [[nodiscard]] static inline Buffer MakeStaging(const BufferSize size) noexcept
        { return Buffer(BufferModel::MakeStaging(size)); }

    /** @brief Destruct the buffer */
    ~Buffer(void) noexcept;

    /** @brief Default constructor */
    Buffer(void) noexcept = default;

    /** @brief Construct a nw buffer using a buffer model */
    Buffer(const BufferModel &model) noexcept;

    /** @brief Move constructor */
    Buffer(Buffer &&other) noexcept = default;

    /** @brief Move assignment */
    Buffer &operator=(Buffer &&other) noexcept = default;
};
