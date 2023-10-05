/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Assert helper
 */

#pragma once

#include "Debug.hpp"

#if KUBE_DEBUG_BUILD // Debug mode
# include "Abort.hpp"
/** @brief Abort only in debug mode */
# define kFDebugAbort(...) kFAbort(__VA_ARGS__)
/** @brief Assert an expression only in debug mode */
# define kFAssert(expression, ...)      kFEnsure(expression, __VA_ARGS__)
# define kFAssertRaw(expression, ...)   kFEnsureRaw(expression, __VA_ARGS__)

/** @brief Write a message to the info log only in debug mode */
# define kFDebugInfo(...)     kFInfo(__VA_ARGS__)
# define kFDebugInfoRaw(...)  kFInfoRaw(__VA_ARGS__)

#else // Release mode
# define kFDebugAbort(...)              static_cast<void>(0)
# define kFAssert(expression, ...)      static_cast<void>(0)
# define kFAssertRaw(expression, ...)   static_cast<void>(0)
# define kFDebugInfo(...)               static_cast<void>(0)
# define kFDebugInfoRaw(...)            static_cast<void>(0)
#endif