/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Interface for child GPU objects
 */

#pragma once

#include <Kube/Core/Utils.hpp>

namespace kF::GPU
{
    class GPU;
    class GPUObject;
    class CachedGPUObject;
}

/** @brief This is the base class of all GPU sub objects
 * This class is not virtual because it is trivially destructible */
class kF::GPU::GPUObject
{
public:
    /** @brief Get global parent GPU */
    [[nodiscard]] inline GPU &parent(void) const noexcept { return Parent(); }

    /** @brief Get global parent GPU */
    [[nodiscard]] static GPU &Parent(void) noexcept;
};

/** @brief This class specializes the GPUObject to hold its parent GPU in its member cache
 * This class is not virtual because it is trivially destructible */
class alignas_eighth_cacheline kF::GPU::CachedGPUObject : public GPUObject
{
public:
    /** @brief Default constructor */
    CachedGPUObject(void) noexcept : _parent(&Parent()) {}

    /** @brief Copy constructor */
    CachedGPUObject(const CachedGPUObject &other) noexcept = default;

    /** @brief Move constructor */
    CachedGPUObject(CachedGPUObject &&other) noexcept = default;

    /** @brief Copy assignment */
    CachedGPUObject &operator=(const CachedGPUObject &other) noexcept = default;

    /** @brief Move assignment */
    CachedGPUObject &operator=(CachedGPUObject &&other) noexcept = default;

    /** @brief Swap two instances */
    void swap(CachedGPUObject &other) noexcept { std::swap(_parent, other._parent); }


    /** @brief Get cached parent GPU */
    [[nodiscard]] GPU &parent(void) const noexcept { return *_parent; }


private:
    GPU *_parent {};

    using GPUObject::parent;
};