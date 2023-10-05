/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetWrite
 */

#pragma once

#include "DescriptorSetWriteModel.hpp"
#include "DescriptorSetCopyModel.hpp"

namespace kF::GPU
{
    struct DescriptorSetUpdate
    {
        /** @brief Write update of a descriptor set */
        static void UpdateWrite(
            const DescriptorSetWriteModel * const writeModelBegin,
            const DescriptorSetWriteModel * const writeModelEnd
        ) noexcept;

        /** @brief Write update of a descriptor set using initializer-list */
        static inline void UpdateWrite(const std::initializer_list<DescriptorSetWriteModel> &writeModels) noexcept
            { UpdateWrite(writeModels.begin(), writeModels.end()); }


        /** @brief Copy update of a descriptor set */
        static void UpdateCopy(
            const DescriptorSetCopyModel * const copyModelBegin,
            const DescriptorSetCopyModel * const copyModelEnd
        ) noexcept;

        /** @brief Copy update of a descriptor set using initializer-list */
        static void UpdateCopy(const std::initializer_list<DescriptorSetCopyModel> &copyModels) noexcept
            { UpdateCopy(copyModels.begin(), copyModels.end()); }


        /** @brief Write & copy update of a descriptor set */
        static void UpdateWriteAndCopy(
            const DescriptorSetWriteModel * const writeModelBegin,
            const DescriptorSetWriteModel * const writeModelEnd,
            const DescriptorSetCopyModel * const copyModelBegin,
            const DescriptorSetCopyModel * const copyModelEnd
        ) noexcept;

        /** @brief Write & copy update of a descriptor set using initializer-lists */
        static void UpdateWriteAndCopy(
            const std::initializer_list<DescriptorSetWriteModel> &writeModels,
            const std::initializer_list<DescriptorSetCopyModel> &copyModels
        ) noexcept
            { UpdateWriteAndCopy(writeModels.begin(), writeModels.end(), copyModels.begin(), copyModels.end()); }
    };
};
