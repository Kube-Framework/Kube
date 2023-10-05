/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ASystem
 */

#pragma once

#include <Kube/Core/Expected.hpp>
#include <Kube/Core/Hash.hpp>
#include <Kube/Flow/Graph.hpp>

#include "PipelineEvent.hpp"

namespace kF::ECS
{
    class Executor;

    /** @brief Pipeline index */
    using PipelineIndex = std::uint32_t;

    namespace Internal
    {
        class ASystem;

        /**
         * @brief Interface class of a system
         * @note Vtable offset is platform specific, this is the only way of
         * ensuring the layout but prevent some optimisation*/
        class ISystem
        {
        public:
            /** @brief Virtual destructor */
            virtual ~ISystem(void) noexcept = default;

            /** @brief Virtual system tick function  */
            [[nodiscard]] virtual bool tick(void) noexcept = 0;

            /** @brief Virtual pipeline name get  */
            [[nodiscard]] virtual std::string_view pipelineName(void) const noexcept = 0;

            /** @brief Virtual system name get  */
            [[nodiscard]] virtual std::string_view systemName(void) const noexcept = 0;
        };
    }
}

/** @brief Abstract class of any system */
class alignas_cacheline kF::ECS::Internal::ASystem : public ISystem
{
public:
    /** @brief Virtual destructor */
    virtual ~ASystem(void) noexcept override = default;

    /** @brief Default constructor */
    ASystem(void) noexcept;

    /** @brief ASystem is not copiable */
    ASystem(const ASystem &other) noexcept = delete;
    ASystem &operator=(const ASystem &other) noexcept = delete;


    /** @brief Virtual system tick function  */
    [[nodiscard]] virtual bool tick(void) noexcept override { return true; }


    /** @brief Get parent executor reference */
    [[nodiscard]] inline Executor &parent(void) const noexcept { return *_parent; }


    /** @brief Get executor pipeline index */
    [[nodiscard]] inline PipelineIndex executorPipelineIndex(void) const noexcept { return _executorPipelineIndex; }


    /** @brief Get internal task graph reference */
    [[nodiscard]] inline Flow::Graph &taskGraph(void) noexcept { return *_graph; }
    [[nodiscard]] inline const Flow::Graph &taskGraph(void) const noexcept { return *_graph; }


    /** @brief Get tick rate */
    [[nodiscard]] inline std::int64_t tickRate(void) const noexcept { return _tickRate; }

    /** @brief Get tick rate */
    [[nodiscard]] inline bool isTimeBound(void) const noexcept { return _isTimeBound; }


    /** @brief Function called whenever executor changes attached pipeline tick rate */
    inline void onTickRateChanged(const std::int64_t tickRate) noexcept { _tickRate = tickRate; }

    /** @brief Set executor pipeline index */
    void queryPipelineIndex(const Core::HashedName pipelineHash) noexcept;


    /** @brief Creates an entity */
    [[nodiscard]] Entity add(void) noexcept;

    /** @brief Creates a range of entities */
    [[nodiscard]] EntityRange addRange(const Entity count) noexcept;

    /** @brief Removes an entity */
    void remove(const Entity entity) noexcept;

    /** @brief Removes a range of entities */
    void removeRange(const EntityRange range) noexcept;

protected:
    /** @brief Get pipeline index from pipeline runtime name */
    [[nodiscard]] Core::Expected<PipelineIndex> getPipelineIndex(const Core::HashedName pipelineHash) const noexcept;

    /** @brief Get opaque system using pipeline index & system hashed name
     *  @return Returns nullptr if system doesn't exist */
    [[nodiscard]] ASystem *getSystemOpaque(const PipelineIndex pipelineIndex, const Core::HashedName systemName) const noexcept;

    /** @brief Send event using pipeline hashed name */
    template<bool RetryOnFailure = true>
    inline void sendEventOpaque(const PipelineIndex pipelineIndex, PipelineEvent &&callback) const noexcept
    {
        if constexpr (RetryOnFailure)
            sendEventOpaqueRetryOnFailure(pipelineIndex, std::move(callback));
        else
            sendEventOpaqueExitOnFailure(pipelineIndex, std::move(callback));
    }

    /** @brief Send event using pipeline hashed name (exit on failure) */
    void sendEventOpaqueExitOnFailure(const PipelineIndex pipelineIndex, PipelineEvent &&callback) const noexcept;

    /** @brief Send event using pipeline hashed name (retry on failure) */
    void sendEventOpaqueRetryOnFailure(const PipelineIndex pipelineIndex, PipelineEvent &&callback) const noexcept;


private:
    // Cacheline 0
    // vtable pointer
    Executor *_parent {};
    PipelineIndex _executorPipelineIndex {};
    bool _isTimeBound {};
    std::int64_t _tickRate {};
    Flow::GraphPtr _graph {};
    Entity _lastEntity {};
    Core::Vector<EntityRange, ECSAllocator> _freeEntities {};
};
static_assert_alignof_cacheline(kF::ECS::Internal::ASystem);
static_assert_sizeof_cacheline(kF::ECS::Internal::ASystem);