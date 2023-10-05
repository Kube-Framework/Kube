/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorPool
 */

#pragma once

#include "Handle.hpp"
#include "DescriptorPoolModel.hpp"
#include "DescriptorSetLayout.hpp"

namespace kF::GPU
{
    class DescriptorPool;
};

/** @brief Abstract an DescriptorPool */
class kF::GPU::DescriptorPool : public Handle<VkDescriptorPool>
{
public:
    /** @brief Initializer list constructor */
    [[nodiscard]] static inline DescriptorPool Make(
        const DescriptorPoolCreateFlags flags,
        const std::uint32_t maxSets,
        const std::initializer_list<DescriptorPoolSize> &poolSizes
    ) noexcept
        { return DescriptorPool(DescriptorPoolModel(flags, maxSets, poolSizes.begin(), poolSizes.end())); }


    /** @brief Destruct the pool */
    ~DescriptorPool(void) noexcept;

    /** @brief Default constructor */
    DescriptorPool(void) noexcept = default;

    /** @brief Construct a new DescriptorPool using DescriptorPool model */
    DescriptorPool(const DescriptorPoolModel &model) noexcept;

    /** @brief Move constructor */
    DescriptorPool(DescriptorPool &&other) noexcept = default;

    /** @brief Move assignment */
    DescriptorPool &operator=(DescriptorPool &&other) noexcept = default;


    /** @brief Allocate a singnle descriptor set from its layout */
    [[nodiscard]] DescriptorSetHandle allocate(const DescriptorSetLayoutHandle &layout) noexcept;

    /** @brief Allocate descriptor sets from descriptor set layouts */
    void allocate(
        const DescriptorSetLayoutHandle * const layoutBengin,
        const DescriptorSetLayoutHandle * const layoutEnd,
        DescriptorSetHandle * const descriptorSetBegin,
        DescriptorSetHandle * const descriptorSetEnd
    ) noexcept;

    /** @brief Deallocate a single descriptor set
     *  @note Only works for pools without FreeDescriptorSet flag */
    inline void deallocate(const DescriptorSetHandle handle) noexcept { deallocate(&handle, &handle + 1); }

    /** @brief Deallocate a range of descriptor sets
     *  @note Only works for pools without FreeDescriptorSet flag */
    void deallocate(const DescriptorSetHandle * const begin, const DescriptorSetHandle * const end) noexcept;


    /** @brief Reset the descriptor pool */
    void reset(void) noexcept;
};