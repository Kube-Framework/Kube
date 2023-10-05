/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Fence
 */

#pragma once

#include "Handle.hpp"

namespace kF::GPU
{
    class Fence;
}

/** @brief Abstraction of a low level system fence */
class kF::GPU::Fence : public Handle<VkFence>
{
public:
    /** @brief Wait fences to be signaled */
    static bool Wait(const FenceHandle * const begin, const FenceHandle * const end,
            const bool waitAll = true, const std::uint64_t timeout = UINT64_MAX) noexcept;

    /** @brief Reset fences to be signaled */
    static void Reset(const FenceHandle * const begin, const FenceHandle * const end) noexcept;


    /** @brief Destroy the fence */
    ~Fence(void) noexcept;

    /** @brief Construct the fence */
    Fence(const bool signaled = true) noexcept;

    /** @brief Move constructor */
    Fence(Fence &&other) noexcept = default;

    /** @brief Move assignment */
    Fence &operator=(Fence &&other) noexcept = default;


    /** @brief Get signaled status */
    [[nodiscard]] bool isSignaled(void) const noexcept;

    /** @brief Wait the fence to be signaled */
    bool wait(const std::uint64_t timeout = UINT64_MAX) const noexcept
        { return Wait(&handle(), &handle() + 1, true, timeout); }

    /** @brief Reset the fence */
    void reset(void) noexcept
        { Reset(&handle(), &handle() + 1); }
};