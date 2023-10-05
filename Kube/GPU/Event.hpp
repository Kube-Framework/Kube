/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Event
 */

#pragma once

#include "Handle.hpp"

namespace kF::GPU
{
    class Event;
}

/** @brief Abstraction of an event */
class kF::GPU::Event : public Handle<VkEvent>
{
public:
    /** @brief Destruct the Event */
    ~Event(void) noexcept;

    /** @brief Construct a Event */
    Event(void) noexcept;

    /** @brief Move constructor */
    Event(Event &&other) noexcept = default;

    /** @brief Move assignment */
    Event &operator=(Event &&other) noexcept = default;


    /** @brief Set the state of the event to signaled  */
    void signal(void) noexcept;

    /** @brief Return current event state (signaled / unsignaled) */
    [[nodiscard]] bool isSignaled(void) const noexcept;

    /** @brief Set the state of the event to unsignaled */
    void reset(void) noexcept;
};