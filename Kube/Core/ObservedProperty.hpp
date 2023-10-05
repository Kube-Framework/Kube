/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Observed property
 */

#pragma once

#include "RemovableDispatcher.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    class ObservedProperty;

    /** @brief Requirements of an observed property type */
    template<typename Type>
    concept ObservedPropertyRequirements
        = (std::is_copy_constructible_v<Type> || std::is_move_constructible_v<Type>)
        || (std::is_copy_assignable_v<Type> || std::is_move_assignable_v<Type>)
        || requires(const Type &lhs, const Type &rhs) { { lhs == rhs } -> std::same_as<bool>; };
}

template<typename Type, kF::Core::StaticAllocatorRequirements Allocator>
class kF::Core::ObservedProperty
{
public:
    /** @brief Property dispatcher */
    using Dispatcher = Core::RemovableDispatcher<void(const Type &)>;

    /** @brief Destructor */
    ~ObservedProperty(void) noexcept = default;

    /** @brief Default constructor */
    inline ObservedProperty(void) noexcept
        requires std::is_default_constructible_v<Type>
        : _value() {}

    /** @brief Move constructor */
    inline ObservedProperty(ObservedProperty &&other) noexcept
        requires std::is_move_constructible_v<Type>
        : _value(std::move(other._value)), _dispatcher(std::move(other._dispatcher)) {}

    /** @brief Copy value constructor */
    explicit inline ObservedProperty(const Type &value) noexcept
        requires std::is_copy_constructible_v<Type>
        : _value(value) {}

    /** @brief Move value constructor */
    explicit inline ObservedProperty(Type &&value) noexcept
        requires std::is_move_constructible_v<Type>
        : _value(std::move(value)) {}

    /** @brief Move assignment */
    inline ObservedProperty &operator=(ObservedProperty &&other) noexcept
        requires std::is_move_assignable_v<Type>
    {
        _value = std::move(other._value);
        _dispatcher = std::move(other._dispatcher);
        return *this;
    }

    /** @brief Copy value assignment */
    inline ObservedProperty &operator=(const Type &value) noexcept
        requires std::is_copy_assignable_v<Type>
    {
        if (_value == value)
            return *this;
        _value = value;
        _dispatcher.dispatch(_value);
        return *this;
    }

    /** @brief Move value assignment */
    inline ObservedProperty &operator=(Type &&value) noexcept
        requires std::is_move_assignable_v<Type>
    {
        if (_value == value)
            return *this;
        _value = std::move(value);
        _dispatcher.dispatch(_value);
        return *this;
    }

    /** @brief Assign operator */
    template<typename Other>
    inline ObservedProperty &operator=(Other &&other) noexcept
        requires (std::is_constructible_v<Type, Other> && std::is_move_assignable_v<Type> && !std::is_same_v<Type, Other>)
        { return *this = Type(std::forward<Other>(other)); }


    /** @brief Explicit bool operator */
    [[nodiscard]] explicit inline operator bool(void) const noexcept
        requires std::is_convertible_v<Type, bool>
        { return _value; }

    /** @brief Implicit cast operator */
    [[nodiscard]] inline operator const Type &(void) const noexcept { return _value; }

    /** @brief Dereference operator */
    [[nodiscard]] inline const Type *operator->(void) const noexcept { return _value; }
    [[nodiscard]] inline const Type &operator*(void) const noexcept { return _value; }

    /** @brief Get value */
    [[nodiscard]] inline const Type &get(void) const noexcept { return _value; }


    /** @brief Dispatch value changes */
    [[nodiscard]] inline Dispatcher &dispatcher(void) noexcept { return _dispatcher; }

    /** @brief Equality operators */
    template<typename Other>
    [[nodiscard]] constexpr bool operator==(const Other &other) const noexcept { return _value == other; }
    template<typename Other>
    [[nodiscard]] constexpr bool operator!=(const Other &other) const noexcept { return _value != other; }
private:
    Type _value;
    Dispatcher _dispatcher {};
};