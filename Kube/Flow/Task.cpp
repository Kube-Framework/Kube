/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow task
 */

#include <Kube/Core/Abort.hpp>

#include "Task.hpp"

using namespace kF;

Flow::Task::~Task(void) noexcept
{
}

Flow::Task &Flow::Task::before(Task &other) noexcept
{
    _linkedTo.push(&other);
    other._linkedFrom.push(this);
    return *this;
}

Flow::Task &Flow::Task::after(Task &other) noexcept
{
    _linkedFrom.push(&other);
    other._linkedTo.push(this);
    return *this;
}

void Flow::Task::reset(void) noexcept
{
    for (Task * const link : _linkedFrom) {
        const auto it = link->_linkedTo.find(this);
        kFEnsure(it != link->_linkedTo.end(), "Flow::Task::reset: Self not found inside linked task");
        link->_linkedTo.erase(it);
    }
    for (Task * const link : _linkedTo) {
        const auto it = link->_linkedFrom.find(this);
        kFEnsure(it != link->_linkedFrom.end(), "Flow::Task::reset: Self not found inside linked task");
        link->_linkedFrom.erase(it);
    }
}