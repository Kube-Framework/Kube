/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Abort helper
 */

#pragma once

#include "Log.hpp"

/** @brief Write a message to the error log then abort the program */
#define kFAbort(...)                    __VA_OPT__(kFError(__VA_ARGS__),) std::terminate()
#define kFAbortRaw(...)                 __VA_OPT__(kFErrorRaw(__VA_ARGS__),) std::terminate()

/** @brief Test an expression and abort if it fails */
#define kFEnsure(expression, ...)       if (!(expression)) [[unlikely]] kFAbort(__VA_OPT__(__VA_ARGS__))
#define kFEnsureRaw(expression, ...)    if (!(expression)) [[unlikely]] kFAbortRaw(__VA_OPT__(__VA_ARGS__))