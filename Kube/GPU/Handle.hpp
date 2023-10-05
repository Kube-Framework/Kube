/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Handle helper
 */

#pragma once

#include <utility>

#include "Base.hpp"
#include "GPUObject.hpp"

namespace kF::GPU
{
    template<typename Type, typename Base>
    class HandleBase;

    /** @brief Handle that inherits from GPUObject */
    template<typename Type>
    struct alignas_eighth_cacheline Handle : public HandleBase<Type, GPUObject> {};

    /** @brief Handle that inherits from CachedGPUObject */
    template<typename Type>
    struct alignas_quarter_cacheline CachedHandle : public HandleBase<Type, CachedGPUObject> {};
}

/** @brief Abstraction of a handle */
template<typename Type, typename Base>
class kF::GPU::HandleBase : public Base
{
public:
    /** @brief Default constructor, does not release handle */
    ~HandleBase(void) noexcept = default;

    /** @brief Default construct the handle */
    HandleBase(void) noexcept = default;

    /** @brief Move constructor */
    HandleBase(HandleBase &&other) noexcept : Base(other) { swap(other); }

    /** @brief Move assignment */
    HandleBase &operator=(HandleBase &&other) noexcept { swap(other); return *this; }


    /** @brief Swap two instances */
    void swap(HandleBase &other) noexcept { std::swap(_handle, other._handle); }


    /** @brief Fast handle check operator */
    operator bool(void) const noexcept { return !isNull(); }

    /** @brief Check if the handle is null */
    [[nodiscard]] bool isNull(void) const noexcept { return _handle == NullHandle; }


    /** @brief Get the Handle */
    [[nodiscard]] Type &handle(void) noexcept { return _handle; }
    [[nodiscard]] const Type &handle(void) const noexcept { return _handle; }

    /** @brief Implicit convertion to handle */
    [[nodiscard]] operator Type(void) const noexcept { return _handle; }

private:
    Type _handle { NullHandle };
};
