/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Semaphore
 */

#pragma once

#include "Handle.hpp"

namespace kF::GPU
{
    class Semaphore;
    class TimelineSemaphore;
}

/** @brief Abstraction of a low-level system binary semaphore */
class kF::GPU::Semaphore : public Handle<SemaphoreHandle>
{
public:
    /** @brief Destruct the semaphore */
    ~Semaphore(void) noexcept;

    /** @brief Construct a semaphore */
    Semaphore(void) noexcept;

    /** @brief Move constructor */
    Semaphore(Semaphore &&other) noexcept = default;

    /** @brief Move constructor */
    Semaphore &operator=(Semaphore &&other) noexcept = default;
};

/** @brief Abstraction of a low-level system timeline semaphore */
class kF::GPU::TimelineSemaphore : public Handle<SemaphoreHandle>
{
public:
    /** @brief Destruct the semaphore */
    ~TimelineSemaphore(void) noexcept;

    /** @brief Construct a semaphore */
    TimelineSemaphore(const std::uint64_t initialValue) noexcept;

    /** @brief Move constructor */
    TimelineSemaphore(TimelineSemaphore &&other) noexcept = default;

    /** @brief Move constructor */
    TimelineSemaphore &operator=(TimelineSemaphore &&other) noexcept = default;


    /** @brief Signal the semaphore */
    void signal(const std::uint64_t value) noexcept;
};