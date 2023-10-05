/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocation
 */

#pragma once

#include "MemoryAllocationModel.hpp"
#include "Handle.hpp"
#include "MemoryAllocator.hpp"

namespace kF::GPU
{
    class MemoryAllocation;
}

/** @brief GPU Memory buffer abstraction */
class kF::GPU::MemoryAllocation : public Handle<MemoryAllocationHandle>
{
public:
    /** @brief Make a memory allocation for a staging buffer and copy its memory */
    template<kF::GPU::MemoryBindable Bindable, std::input_iterator Iterator>
    [[nodiscard]] static inline MemoryAllocation MakeStaging(
        const Bindable &toBind,
        const Iterator dataBegin,
        const Iterator dataEnd
    ) noexcept;

    /** @brief Make a memory allocation for a staging buffer */
    template<kF::GPU::MemoryBindable Bindable>
    [[nodiscard]] static inline MemoryAllocation MakeStaging(const Bindable &toBind) noexcept
        { return MemoryAllocation(MemoryAllocationModel::MakeStaging(toBind)); }

    /** @brief Make a local (GPU only) memory allocation and bind a vertex buffer */
    template<MemoryBindable Bindable>
    [[nodiscard]] static inline MemoryAllocation MakeLocal(const Bindable &toBind) noexcept
        { return MemoryAllocation(MemoryAllocationModel::MakeLocal(toBind)); }


    /** @brief Destruct the buffer */
    ~MemoryAllocation(void) noexcept;

    /** @brief Default constructor */
    MemoryAllocation(void) noexcept = default;

    /** @brief Construct a nw buffer using a buffer model */
    MemoryAllocation(const MemoryAllocationModel &model) noexcept;

    /** @brief Move constructor */
    MemoryAllocation(MemoryAllocation &&other) noexcept = default;

    /** @brief Move assignment */
    MemoryAllocation &operator=(MemoryAllocation &&other) noexcept = default;


    /** @brief Map iterator range to memory */
    template<std::input_iterator Iterator>
    void memoryMap(const Iterator dataBegin, const Iterator dataEnd) noexcept;


    /** @brief Begin memory mapping (must be followed by endMap) */
    template<typename Type>
    [[nodiscard]] Type *beginMemoryMap(void) noexcept
        { return reinterpret_cast<Type *>(beginMemoryMapImpl()); }

    /** @brief End memory mapping */
    void endMemoryMap(void) noexcept;

private:
    /** @brief Implementation of beginMemoryMap */
    [[nodiscard]] std::uint8_t *beginMemoryMapImpl(void) noexcept;
};

#include "MemoryAllocation.ipp"