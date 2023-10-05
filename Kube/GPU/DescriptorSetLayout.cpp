/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: DescriptorSetLayout
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSetLayoutBindingFlagsModel.hpp"

using namespace kF;

GPU::DescriptorSetLayout GPU::DescriptorSetLayout::Make(
    const DescriptorSetLayoutCreateFlags flags,
    const std::initializer_list<DescriptorSetLayoutBinding> &bindings,
    const std::initializer_list<DescriptorBindingFlags> &bindingFlags
) noexcept
{
    const DescriptorSetLayoutBindingFlagsModel flagsModel(bindingFlags.begin(), bindingFlags.end());
    DescriptorSetLayoutModel model(flags, bindings.begin(), bindings.end());
    model.pNext = &flagsModel;
    return DescriptorSetLayout(model);
}

GPU::DescriptorSetLayout::~DescriptorSetLayout(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyDescriptorSetLayout(parent().logicalDevice(), handle(), nullptr);
}

GPU::DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutModel &model) noexcept
{
    if (const auto res = ::vkCreateDescriptorSetLayout(parent().logicalDevice(), &model, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::DescriptorSetLayout: Couldn't create descriptor set layout '", ErrorMessage(res), '\'');
}
