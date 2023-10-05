/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Removable Dispatcher
 */

#pragma once

#include "FunctorUtils.hpp"
#include "Vector.hpp"
#include "SharedPtr.hpp"
#include "DispatcherSlot.hpp"

namespace kF::Core
{
    template<typename Signature, FunctorRequirements InternalFunctor, StaticAllocatorRequirements Allocator>
    class RemovableDispatcherDetails;
}

/** @brief Fast event dispatcher */
template<typename Return, typename... Args, kF::Core::FunctorRequirements InternalFunctor, kF::Core::StaticAllocatorRequirements Allocator>
class alignas_eighth_cacheline kF::Core::RemovableDispatcherDetails<Return(Args...), InternalFunctor, Allocator>
{
public:
    /** @brief Functor type */
    using Functor = InternalFunctor;

    /** @brief Functor handle */
    using Handle = DispatcherSlot::Handle;

    /** @brief Functor handle */
    using ArgsTuple = std::tuple<Args...>;

    /** @brief Removable dispatcher instance */
    struct alignas_half_cacheline Instance
    {
        // @todo make a single allocation removable dispatcher
        Vector<InternalFunctor, Allocator> functors {};
        Vector<Handle, Allocator> freeList {};


        /** @brief Remove a slot from its handle */
        inline void remove(const Handle handle) noexcept
        {
            kFAssert(handle, "RemovableDispatcherDetails::remove: Can't remove null handle");
            const auto index = handle - 1u;
            functors[index].release();
            freeList.push(index);
        }
    };

    /** @brief Shared dispatcher instance */
    using SharedInstance = SharedPtr<Instance, Allocator>;


    /** @brief Destructor */
    inline ~RemovableDispatcherDetails(void) noexcept = default;

    /** @brief Default constructor */
    inline RemovableDispatcherDetails(void) noexcept = default;

    /** @brief Move constructor */
    inline RemovableDispatcherDetails(RemovableDispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Move assignment*/
    inline RemovableDispatcherDetails &operator=(RemovableDispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Internal functor count */
    [[nodiscard]] inline auto count(void) const noexcept { return _sharedInstance->functors.size() - _sharedInstance->freeList.size(); }


    /** @brief Add a functor to dispatch list */
    template<typename Functor>
    [[nodiscard]] inline DispatcherSlot add(Functor &&functor) noexcept
    {
        auto &instance = *_sharedInstance;
        InternalFunctor *internal;
        Handle index;
        if (!instance.freeList.empty()) {
            index = instance.freeList.back();
            instance.freeList.pop();
            internal = &instance.functors[index];
        } else {
            index = instance.functors.size();
            internal = &instance.functors.push();
        }
        internal->prepare(std::forward<Functor>(functor));
        return makeDispatcherSlot(index + 1);
    }

    /** @brief Add a member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    [[nodiscard]] inline DispatcherSlot add(ClassType &&classType) noexcept
    {
        auto &instance = *_sharedInstance;
        InternalFunctor *functor;
        Handle index;
        if (!instance.freeList.empty()) {
            index = instance.freeList.back();
            instance.freeList.pop();
            functor = &instance.functors[index];
        } else {
            index = instance.functors.size();
            functor = &instance.functors.push();
        }
        functor->template prepare<MemberFunction>(std::forward<ClassType>(classType));
        return makeDispatcherSlot(index + 1);
    }

    /** @brief Add a free function to dispatch list */
    template<auto FreeFunction>
    [[nodiscard]] inline DispatcherSlot add(void) noexcept
    {
        auto &instance = *_sharedInstance;
        InternalFunctor *functor;
        Handle index;
        if (!instance.freeList.empty()) {
            index = instance.freeList.back() - 1;
            instance.freeList.pop();
            functor = &instance.functors[index];
        } else {
            index = instance.functors.size();
            functor = &instance.functors.push();
        }
        functor->template prepare<FreeFunction>();
        return makeDispatcherSlot(index + 1);
    }


    /** @brief Dispatch every internal functors */
    inline void dispatch(Args ...args) const noexcept
    {
        for (auto &functor : _sharedInstance->functors) {
            if (functor)
                functor(std::forward<Args>(args)...);
        }
    }

    /** @brief Dispatch every internal functors with a given callback to receive the return value of each functor */
    template<typename Callback>
        requires (!std::is_same_v<Return, void> && std::invocable<Callback, Return>)
    inline void dispatch(Callback &&callback, Args ...args) const noexcept
    {
        for (auto &functor : _sharedInstance->functors) {
            if (functor)
                callback(functor(std::forward<Args>(args)...));
        }
    }

private:
    /** @brief Construct a dispatcher from an handle */
    [[nodiscard]] inline DispatcherSlot makeDispatcherSlot(const Handle handle) noexcept
    {
        constexpr auto DisconnectFunc = [](void *data, const Handle handle) {
            auto &sharedInstance = *reinterpret_cast<SharedInstance *>(&data);
            // Remove slot
            sharedInstance->remove(handle);
            // Decrement reference count
            sharedInstance.release();
        };

        // Increment reference count
        static_assert(sizeof(SharedInstance) == sizeof(void*), "RemovableDispatcherDetails: Implementation requires SharedPtr 8 byte sized");
        void *data {};
        new (&data) SharedInstance(_sharedInstance);

        // Construct dispatcher slot
        return DispatcherSlot::Make(DisconnectFunc, data, handle);
    }


    SharedInstance _sharedInstance { SharedInstance::Make() };
};