/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#include "Log.hpp"
#include "StringUtils.hpp"

template<kF::Core::Log::HasNewLine NewLine, typename ...Args>
inline void kF::Core::Log::log(Args &&...args) noexcept
{
    (*_target << ... << std::forward<Args>(args));

    if constexpr (NewLine == HasNewLine::Yes)
        *_target << std::endl;
}
