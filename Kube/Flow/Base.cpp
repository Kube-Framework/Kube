/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Flow base
 */

#include "Base.hpp"

#include <Kube/Core/Abort.hpp>

using namespace kF;

#if KUBE_PLATFORM_WINDOWS
#include <Windows.h>
struct WindowsTimer
{
    HANDLE handle;

    WindowsTimer(void) noexcept
    {
        handle = CreateWaitableTimer(NULL, FALSE, NULL);
        kFEnsure(handle,
            "ECS::Executor::PreciseSleep: Couldn't create windows timer handle");
    }

    ~WindowsTimer(void) noexcept { CloseHandle(handle); }
};

void Flow::PreciseSleep(const std::int64_t nanoseconds) noexcept
{
    static thread_local WindowsTimer WindowsTimer {};

    // Setup timer & wait
    LARGE_INTEGER timeDef;
    timeDef.QuadPart = -nanoseconds / 100;
    if (SetWaitableTimer(WindowsTimer.handle, &timeDef, 0, nullptr, nullptr, false)) [[likely]]
        WaitForSingleObject(WindowsTimer.handle, INFINITE);
}
#else
#include <time.h>
void Flow::PreciseSleep(const std::int64_t nanoseconds) noexcept
{
    const struct timespec spec {
        .tv_sec = time_t {},
        .tv_nsec = nanoseconds
    };
    struct timespec rem {};
    nanosleep(&spec, &rem);
}
#endif