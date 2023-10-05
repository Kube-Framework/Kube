/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Event
 */

#include <Kube/Core/Abort.hpp>

#include "GPU.hpp"
#include "Event.hpp"

using namespace kF;

GPU::Event::~Event(void) noexcept
{
    if (handle()) [[likely]]
        ::vkDestroyEvent(parent().logicalDevice(), handle(), nullptr);
}

GPU::Event::Event(void) noexcept
{
    const VkEventCreateInfo eventCreateInfo {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = VkEventCreateFlags()
    };

    if (const auto res = ::vkCreateEvent(parent().logicalDevice(), &eventCreateInfo, nullptr, &handle()); res != VK_SUCCESS)
        kFAbort("GPU::Event: Couldn't create event '", ErrorMessage(res), '\'');
}

void GPU::Event::signal(void) noexcept
{
    if (const auto res = ::vkSetEvent(parent().logicalDevice(), handle()); res != VK_SUCCESS)
        kFAbort("GPU::Event::set: Couldn't set event status '", ErrorMessage(res), '\'');
}

bool GPU::Event::isSignaled(void) const noexcept
{
    const auto res = ::vkGetEventStatus(parent().logicalDevice(), handle());

    if (res == VK_EVENT_SET)
        return true;
    else if (res == VK_EVENT_RESET)
        return false;
    else
        kFAbort("GPU::Event::isSignaled: Couldn't get event status '", ErrorMessage(res), '\'');
}

void GPU::Event::reset(void) noexcept
{
    if (const auto res = ::vkResetEvent(parent().logicalDevice(), handle()); res != VK_SUCCESS)
        kFAbort("GPU::Event::reset: Couldn't reset event '", ErrorMessage(res), '\'');
}