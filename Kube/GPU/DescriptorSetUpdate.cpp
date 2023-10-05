/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriporSetUpdate
 */

#include "GPU.hpp"
#include "DescriptorSetUpdate.hpp"

using namespace kF;

void GPU::DescriptorSetUpdate::UpdateWrite(
        const DescriptorSetWriteModel * const writeModelBegin, const DescriptorSetWriteModel * const writeModelEnd) noexcept
{
    vkUpdateDescriptorSets(
        GPUObject::Parent().logicalDevice(),
        Core::Distance<std::uint32_t>(writeModelBegin, writeModelEnd),
        writeModelBegin,
        0,
        nullptr
    );
}

/** @brief Copy update of a descriptor set */
void GPU::DescriptorSetUpdate::UpdateCopy(
        const DescriptorSetCopyModel * const copyModelBegin, const DescriptorSetCopyModel * const copyModelEnd) noexcept
{
    vkUpdateDescriptorSets(
        GPUObject::Parent().logicalDevice(),
        0,
        nullptr,
        Core::Distance<std::uint32_t>(copyModelBegin, copyModelEnd), copyModelBegin
    );
}

/** @brief Write & copy update of a descriptor set */
void GPU::DescriptorSetUpdate::UpdateWriteAndCopy(
        const DescriptorSetWriteModel * const writeModelBegin, const DescriptorSetWriteModel * const writeModelEnd,
        const DescriptorSetCopyModel * const copyModelBegin, const DescriptorSetCopyModel * const copyModelEnd) noexcept
{
    vkUpdateDescriptorSets(
        GPUObject::Parent().logicalDevice(),
        Core::Distance<std::uint32_t>(writeModelBegin, writeModelEnd),
        writeModelBegin,
        Core::Distance<std::uint32_t>(copyModelBegin, copyModelEnd),
        copyModelBegin
    );
}