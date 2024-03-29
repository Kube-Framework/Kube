/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Event system
 */

#pragma once

#include <Kube/ECS/System.hpp>

#include "EventQueue.hpp"

union SDL_Event;

namespace kF::UI
{
    class EventSystem;

    /** @brief Event pipeline, encapsulate event systems */
    using EventPipeline = ECS::PipelineTag<"EventPipeline">;
}

/** @brief Event system, responsible for user input dispatching */
class alignas_double_cacheline kF::UI::EventSystem : public ECS::System<"EventSystem", EventPipeline>
{
public:
    /** @brief Virtual destructor */
    virtual ~EventSystem(void) noexcept override = default;

    /** @brief Constructor */
    EventSystem(void) noexcept;


    /** @brief Virtual tick callback */
    [[nodiscard]] virtual bool tick(void) noexcept override;


    /** @brief Add an event queue of a specific event type */
    template<kF::UI::EventRequirements EventType>
    [[nodiscard]] EventQueuePtr<EventType> addEventQueue(void) noexcept;


private:
    /** @brief Collect all events */
    void collectEvents(void) noexcept;

    /** @brief Interpret a single event */
    void interpretEvent(const SDL_Event &event) noexcept;

    /** @brief Dispatch all collected events */
    void dispatchEvents(void) noexcept;


    Point _lastMousePosition {};
    GPU::Extent2D _resizeExtent {};
    Button _buttons {};
    Modifier _modifiers {};
    Core::Vector<MouseEvent, EventAllocator> _mouseEvents {};
    Core::Vector<WheelEvent, EventAllocator> _wheelEvents {};
    Core::Vector<KeyEvent, EventAllocator> _keyEvents {};
    Core::Vector<TextEvent, EventAllocator> _textEvents {};
    Core::Vector<EventQueuePtr<MouseEvent>, EventAllocator> _mouseQueues {};
    Core::Vector<EventQueuePtr<WheelEvent>, EventAllocator> _wheelQueues {};
    Core::Vector<EventQueuePtr<KeyEvent>, EventAllocator> _keyQueues {};
    Core::Vector<EventQueuePtr<TextEvent>, EventAllocator> _textQueues {};
};

#include "EventSystem.ipp"