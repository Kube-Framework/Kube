/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#pragma once

#include <ostream>
#include <string_view>

#include "Utils.hpp"

namespace kF::Core
{
    class Log;
}

/** @brief Log wrapper */
class kF::Core::Log
{
public:
    /** @brief New line option */
    enum class HasNewLine : bool { No = false, Yes = true };

    /** @brief Destructor */
    inline ~Log(void) noexcept = default;

    /** @brief Constructor */
    inline Log(std::ostream &target) noexcept : _target(&target) {}

    /** @brief Move constructor */
    inline Log(Log &&other) noexcept : _target(other._target) {}

    /** @brief Move assignment */
    inline Log &operator=(Log &&other) noexcept { _target = other._target; return *this; }


    /** @brief Write a message to the target ostream
     *  @tparam AddNewLine If 'Yes', adds 'std::endl' to the message */
    template<HasNewLine NewLine = HasNewLine::Yes, typename ...Args>
    void log(Args &&...args) noexcept;

    /** @brief Write a line break and flush */
    inline void log(void) noexcept { *_target << std::endl; }


    /** @brief Flush target */
    inline void flush(void) const noexcept { _target->flush(); }


private:
    std::ostream *_target {};
};

namespace kF::Core
{
    extern Log InfoLog;
    extern Log ErrorLog;
}

/** @brief Write a message to the info log */
# define kFInfo(...)     kF::Core::InfoLog.log __VA_OPT__(<kF::Core::Log::HasNewLine::Yes>) ( __VA_OPT__(__VA_ARGS__) )
# define kFInfoRaw(...)  kF::Core::InfoLog.log __VA_OPT__(<kF::Core::Log::HasNewLine::No>) ( __VA_OPT__(__VA_ARGS__) )

/** @brief Write a message to the error log */
#define kFError(...)     kF::Core::ErrorLog.log __VA_OPT__(<kF::Core::Log::HasNewLine::Yes>) ( __VA_OPT__(__VA_ARGS__) )
#define kFErrorRaw(...)  kF::Core::ErrorLog.log __VA_OPT__(<kF::Core::Log::HasNewLine::No>) ( __VA_OPT__(__VA_ARGS__) )

/** @brief String view overload */
std::ostream &operator<<(std::ostream &lhs, const std::string_view &rhs) noexcept;

#include "Log.ipp"