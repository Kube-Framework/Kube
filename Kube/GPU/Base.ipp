template<typename Container, typename ...FunctionArgs, typename ...Args>
inline VkResult kF::GPU::Internal::FillVkContainer(Container &container, VkResult(*function)(FunctionArgs...), Args &&...args) noexcept
{
    std::uint32_t count = 0;
    VkResult res = (*function)(std::forward<Args>(args)..., &count, nullptr);
    if (res != VK_SUCCESS || !count)
        return res;
    container.resize(count);
    return (*function)(std::forward<Args>(args)..., &count, container.data());
}

template<typename Container, typename ...FunctionArgs, typename ...Args>
inline void kF::GPU::Internal::FillVkContainer(Container &container, void(*function)(FunctionArgs...), Args &&...args) noexcept
{
    std::uint32_t count = 0;
    (*function)(std::forward<Args>(args)..., &count, nullptr);
    if (!count) [[unlikely]]
        return;
    container.resize(count);
    (*function)(std::forward<Args>(args)..., &count, container.data());
}