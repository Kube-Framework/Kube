/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#include <iostream>

#include "Log.hpp"

using namespace kF;

Core::Log Core::InfoLog(std::cout);
Core::Log Core::ErrorLog(std::cerr);

std::ostream &operator<<(std::ostream &lhs, const std::string_view &rhs) noexcept
{
    lhs.write(rhs.data(), std::streamsize(rhs.length()));
    return lhs;
}
