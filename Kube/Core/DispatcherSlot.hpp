/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher Slot
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    class DispatcherSlot;
}

/** @brief Removable dispatcher handle */
class kF::Core::DispatcherSlot
{
public:
    /** @brief Dispatcher handle */
    using Handle = std::uint32_t;

    /** @brief Disconnect functor */
    using DisconnectFunc = void(*)(void * const, const Handle);


    /** @brief Forward to private constructor to prevent misusage */
    [[nodiscard]] static inline DispatcherSlot Make(const DisconnectFunc disconnectFunc, void * const data, const Handle handle) noexcept
        { return DispatcherSlot(disconnectFunc, data, handle); }


    /** @brief Destructor */
    inline ~DispatcherSlot(void) noexcept { if (_disconnectFunc) _disconnectFunc(_data, _handle); }

    /** @brief Dispatcher slot*/
    inline DispatcherSlot(void) noexcept = default;

    /** @brief DispatcherSlot is not copiable */
    DispatcherSlot(const DispatcherSlot &other) noexcept = delete;
    DispatcherSlot &operator=(const DispatcherSlot &other) noexcept = delete;

    /** @brief Move constructor */
    DispatcherSlot(DispatcherSlot &&other) noexcept;

    /** @brief Move assignment */
    DispatcherSlot &operator=(DispatcherSlot &&other) noexcept;


    /** @brief Boolean operator */
    [[nodiscard]] inline operator bool(void) const noexcept { return _disconnectFunc; }

private:
    /** @brief Private constructor */
    DispatcherSlot(const DisconnectFunc disconnectFunc, void * const data, const Handle handle) noexcept;


    DisconnectFunc _disconnectFunc {};
    void *_data {};
    Handle _handle {};
};

#include "DispatcherSlot.ipp"