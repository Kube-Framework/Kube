/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: BufferModel
 */

inline kF::GPU::BufferModel kF::GPU::BufferModel::MakeExclusive(const BufferSize size, const BufferUsageFlags customUsage) noexcept
{
    return BufferModel(
        BufferCreateFlags::None, size,
        customUsage,
        SharingMode::Exclusive
    );
}

inline kF::GPU::BufferModel kF::GPU::BufferModel::MakeStaging(const BufferSize size) noexcept
{
    return MakeExclusive(size, BufferUsageFlags::TransferSrc);
}
