/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocationModel
 */

template<kF::GPU::MemoryBindable Bindable>
kF::GPU::MemoryAllocationModel::MemoryAllocationModel(const Bindable &toBind, const MemoryUsage usage) noexcept
    :   _bindType(std::is_convertible_v<Bindable, BufferHandle> ? BindType::Buffer : BindType::Image),
        _usage(usage)
{
    if constexpr (std::is_convertible_v<Bindable, BufferHandle>)
        _data.buffer = toBind;
    else
        _data.image = toBind;
}