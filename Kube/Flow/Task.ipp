/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow task
 */

#include "Task.hpp"

inline kF::Flow::Task::Work::~Work(void) noexcept
{
    switch (type) {
    case WorkType::Static:
        staticWork.~StaticWork();
        break;
    case WorkType::Switch:
        switchWork.~SwitchWork();
        break;
    case WorkType::Graph:
        graphWork.~GraphWork();
        break;
    default:
        break;
    }
}

template<typename Type>
inline kF::Flow::Task::Work::Work(Type &&value) noexcept
{
    if constexpr (std::is_constructible_v<GraphWork, Type>) {
        type = WorkType::Graph;
        new (&graphWork) GraphWork(std::forward<Type>(value));
    } else if constexpr (Core::InvocableRequirements<Type, std::size_t>) {
        type = WorkType::Switch;
        new (&switchWork) SwitchWork(std::forward<Type>(value));
    } else if constexpr (Core::InvocableRequirements<Type, void>) {
        type = WorkType::Static;
        new (&staticWork) StaticWork(std::forward<Type>(value));
    }
}