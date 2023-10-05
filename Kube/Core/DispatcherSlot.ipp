/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher Slot
 */

#include "DispatcherSlot.hpp"

inline kF::Core::DispatcherSlot::DispatcherSlot(const DisconnectFunc disconnectFunc, void * const data, const Handle handle) noexcept
    : _disconnectFunc(disconnectFunc)
    , _data(data)
    , _handle(handle)
{
}

inline kF::Core::DispatcherSlot::DispatcherSlot(DispatcherSlot &&other) noexcept
    : _disconnectFunc(other._disconnectFunc), _data(other._data), _handle(other._handle)
{
    other._disconnectFunc = {};
    other._data = {};
    other._handle = {};
}

inline kF::Core::DispatcherSlot &kF::Core::DispatcherSlot::operator=(DispatcherSlot &&other) noexcept
{
    std::swap(_disconnectFunc, other._disconnectFunc);
    std::swap(_data, other._data);
    std::swap(_handle, other._handle);
    return *this;
}