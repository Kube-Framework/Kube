/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#include <iostream>
#include <iomanip>
#include <chrono>

#include "Log.hpp"

using namespace kF;

Core::Log Core::InfoLog(std::cout);
Core::Log Core::ErrorLog(std::cerr);

Core::Log &Core::Log::logRfc3339(void) noexcept
{
    // Get the current date and time
    auto now = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Format the time into the desired string format
    const std::tm timeInfo = *std::localtime(&currentTime);
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    // Print the formatted time
    *_target << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S.") << std::setfill('0') << std::setw(6) << milliseconds << "Z ";
    return *this;
}

std::ostream &operator<<(std::ostream &lhs, const std::string_view &rhs) noexcept
{
    lhs.write(rhs.data(), std::streamsize(rhs.length()));
    return lhs;
}
