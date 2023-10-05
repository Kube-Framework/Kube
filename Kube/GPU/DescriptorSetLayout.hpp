/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetLayout
 */

#pragma once

#include "Handle.hpp"
#include "DescriptorSetLayoutModel.hpp"

namespace kF::GPU
{
    class DescriptorSetLayout;
};

/** @brief Abstract an DescriptorSetLayout */
class kF::GPU::DescriptorSetLayout : public Handle<DescriptorSetLayoutHandle>
{
public:
    /** @brief Initializer-list constructor */
    [[nodiscard]] static inline DescriptorSetLayout Make(
        const DescriptorSetLayoutCreateFlags flags,
        const std::initializer_list<DescriptorSetLayoutBinding> &bindings
    ) noexcept
        { return DescriptorSetLayout(DescriptorSetLayoutModel(flags, bindings.begin(), bindings.end())); }


    /** @brief Initializer-list constructor */
    [[nodiscard]] static DescriptorSetLayout Make(
        const DescriptorSetLayoutCreateFlags flags,
        const std::initializer_list<DescriptorSetLayoutBinding> &bindings,
        const std::initializer_list<DescriptorBindingFlags> &bindingFlags
    ) noexcept;


    /** @brief Destructor */
    ~DescriptorSetLayout(void) noexcept;

    /** @brief Default constructor */
    DescriptorSetLayout(void) noexcept = default;

    /** @brief Construct a new DescriptorSetLayout using DescriptorSetLayout model */
    DescriptorSetLayout(const DescriptorSetLayoutModel &model) noexcept;

    /** @brief Move constructor */
    DescriptorSetLayout(DescriptorSetLayout &&other) noexcept = default;

    /** @brief Move assignment */
    DescriptorSetLayout &operator=(DescriptorSetLayout &&other) noexcept = default;
};
