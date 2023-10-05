/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Buffer model
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    struct BufferModel;
}

/** @brief Describe how to create a buffer */
struct kF::GPU::BufferModel : public VkBufferCreateInfo
{

    /** @brief Helper used to create an excluive buffer model */
    [[nodiscard]] static BufferModel MakeExclusive(const BufferSize size, const BufferUsageFlags customUsage) noexcept;

    /** @brief Helper used to create a staging buffer model */
    [[nodiscard]] static BufferModel MakeStaging(const BufferSize size) noexcept;


    /** @brief Destructor */
    ~BufferModel(void) noexcept = default;

    /** @brief Initialize constructor */
    BufferModel(const BufferCreateFlags flags_, const BufferSize size_, const BufferUsageFlags usage_, const SharingMode sharingMode_ = SharingMode::Exclusive,
            const std::uint32_t * const queueFamilyIndexBegin = nullptr, const std::uint32_t * const queueFamilyIndexEnd = nullptr) noexcept
        : VkBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = ToFlags(flags_),
            .size = size_,
            .usage = ToFlags(usage_),
            .sharingMode = static_cast<VkSharingMode>(sharingMode_),
            .queueFamilyIndexCount = Core::Distance<std::uint32_t>(queueFamilyIndexBegin, queueFamilyIndexEnd),
            .pQueueFamilyIndices = queueFamilyIndexBegin
        } {}

    /** @brief POD semantics */
    BufferModel(const BufferModel &other) noexcept = default;
    BufferModel(BufferModel &&other) noexcept = default;
    BufferModel &operator=(const BufferModel &other) noexcept = default;
    BufferModel &operator=(BufferModel &&other) noexcept = default;
};

#include "BufferModel.ipp"