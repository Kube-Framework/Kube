/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocationModel
 */

#pragma once

#include "Base.hpp"

namespace kF::GPU
{
    class MemoryAllocationModel;

    template<typename Type>
    concept MemoryBindable = std::convertible_to<Type, BufferHandle> || std::convertible_to<Type, ImageHandle>;
}

class kF::GPU::MemoryAllocationModel
{
public:
    /** @brief Type of internal data */
    enum class BindType : std::uint32_t
    {
        Buffer = 0u,
        Image
    };

    /** @brief Can either contain a buffer or an image */
    union Data
    {
        BufferHandle buffer { NullHandle };
        ImageHandle image;
    };


    /** @brief Make a shared memory allocation model */
    template<MemoryBindable Bindable>
    [[nodiscard]] static inline MemoryAllocationModel MakeStaging(const Bindable &toBind) noexcept
        { return MemoryAllocationModel(toBind, MemoryUsage::CpuToGpu); }

    /** @brief Make a local (GPU only) memory allocation model */
    template<MemoryBindable Bindable>
    [[nodiscard]] static inline MemoryAllocationModel MakeLocal(const Bindable &toBind) noexcept
        { return MemoryAllocationModel(toBind, MemoryUsage::GpuOnly); }


    /** @brief Destructor */
    ~MemoryAllocationModel(void) noexcept = default;

    /** @brief Construct a new memory allocation */
    template<kF::GPU::MemoryBindable Bindable>
    MemoryAllocationModel(const Bindable &toBind, const MemoryUsage usage) noexcept;

    /** @brief Copy constructor */
    MemoryAllocationModel(const MemoryAllocationModel &other) noexcept = default;

    /** @brief Copy assignment */
    MemoryAllocationModel &operator=(const MemoryAllocationModel &other) noexcept = default;


    /** @brief Get the type of memory */
    [[nodiscard]] BindType bindType(void) const noexcept { return _bindType; }

    /** @brief Retreive data as a buffer */
    [[nodiscard]] BufferHandle buffer(void) const noexcept { return _data.buffer; }

    /** @brief Retreive data as an image */
    [[nodiscard]] ImageHandle image(void) const noexcept { return _data.image; }


    /** @brief Get the usage of memory */
    [[nodiscard]] MemoryUsage memoryUsage(void) const noexcept { return _usage; }

private:
    Data _data {};
    BindType _bindType { BindType::Buffer };
    MemoryUsage _usage { MemoryUsage::Unknown };
};

#include "MemoryAllocationModel.ipp"