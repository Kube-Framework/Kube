/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of platform compile-time data
 */

#include "Platform.hpp"

#if KUBE_PLATFORM_WINDOWS
# include <windows.h>
#else
# include <unistd.h>
#endif

using namespace kF;

std::size_t Core::Platform::GetPageSize(void) noexcept
{
    static const std::size_t PageSize = [] {
        #if KUBE_PLATFORM_WINDOWS
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            return static_cast<std::size_t>(sysInfo.dwPageSize);
        #else
            return static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
        #endif
    }();

    return PageSize;
}