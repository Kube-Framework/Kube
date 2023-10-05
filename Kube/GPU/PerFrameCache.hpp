/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PerFrameCache
 */

#pragma once

#include <Kube/Core/Vector.hpp>

#include "Base.hpp"

namespace kF::GPU
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
    class PerFrameCache;
}


template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = kF::Core::DefaultStaticAllocator>
class alignas_quarter_cacheline kF::GPU::PerFrameCache
{
public:
    /** @brief Destructor */
    inline ~PerFrameCache(void) noexcept { release<false>(); }

    /** @brief Constructor */
    inline PerFrameCache(void) noexcept = default;

    /** @brief Resize default constructor */
    inline PerFrameCache(const FrameIndex count) noexcept { resize(count); }

    /** @brief Resize copy constructor */
    inline PerFrameCache(const FrameIndex count, const Type &value) noexcept { resize(count, value); }

    /** @brief Resize initializer constructor */
    template<typename Initializer>
    inline PerFrameCache(const FrameIndex count, Initializer &&initializer) noexcept { resize(count, std::forward<Initializer>(initializer)); }

    /** @brief PerFrameCache is not copiable */
    PerFrameCache(const PerFrameCache &other) noexcept = delete;
    PerFrameCache &operator=(const PerFrameCache &other) noexcept = delete;

    /** @brief Move constructor */
    PerFrameCache(PerFrameCache &&other) noexcept = default;

    /** @brief Move assignment */
    PerFrameCache &operator=(PerFrameCache &&other) noexcept = default;


    /** @brief Get frame count */
    [[nodiscard]] inline FrameIndex count(void) const noexcept { return _capacity; }


    /** @brief Begin / End iterators */
    [[nodiscard]] Type *begin(void) noexcept { return _data; }
    [[nodiscard]] const Type *begin(void) const noexcept { return _data; }
    [[nodiscard]] Type *end(void) noexcept { return _data + _capacity; }
    [[nodiscard]] const Type *end(void) const noexcept { return _data + _capacity; }


    /** @brief Get the cache at given index */
    [[nodiscard]] inline Type &at(const FrameIndex index) noexcept { return _data[index]; }
    [[nodiscard]] inline const Type &at(const FrameIndex index) const noexcept { return _data[index]; }

    /** @brief Get the current cache */
    [[nodiscard]] inline Type &current(void) noexcept { return at(currentFrame()); }
    [[nodiscard]] inline const Type &current(void) const noexcept { return at(currentFrame()); }


    /** @brief Get the current frame index */
    [[nodiscard]] inline FrameIndex currentFrame(void) const noexcept { return _index; }

    /** @brief Set the current frame index */
    inline void setCurrentFrame(const FrameIndex frameIndex) noexcept { _index = frameIndex; }

    /** @brief Resize to frame count default-initializing caches */
    void resize(const FrameIndex count) noexcept;

    /** @brief Resize to frame count copy-initializing caches */
    void resize(const FrameIndex count, const Type &value) noexcept;

    /** @brief Resize to frame count and initialize caches through a custom initializer
     *  @note The initializer can take one optional index as function parameter */
    template<typename Initializer>
        requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, kF::GPU::FrameIndex>
    void resize(const FrameIndex count, Initializer &&initializer) noexcept;


    /** @brief Release caches */
    template<bool ResetMembers = true>
    void release(void) noexcept;

private:
    /** @brief Resize to frame count without initializing caches */
    void reserve(const FrameIndex count) noexcept;

    Type *_data {};
    FrameIndex _capacity {};
    FrameIndex _index {};
};

#include "PerFrameCache.ipp"
