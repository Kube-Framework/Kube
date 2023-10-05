/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Expected error handling abstraction
 */

#pragma once

#include "Log.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, typename ErrorType>
        requires (!std::is_same_v<Type, ErrorType> && !std::is_reference_v<Type> && !std::is_reference_v<ErrorType>)
    class Expected;
};

/** @brief Expected class is an error type forwarding.
 *  Member function 'success' gives the state of value / error
 *  'value' / 'error' gives the instance of the value / error type */
template<typename Type, typename ErrorType = void>
    requires (!std::is_same_v<Type, ErrorType> && !std::is_reference_v<Type> && !std::is_reference_v<ErrorType>)
class kF::Core::Expected
{
public:
    /** @brief Detects if an error type is required */
    static constexpr bool HasErrorType = !std::is_same_v<ErrorType, void>;


    /** @brief Fallback into dummy type in case of a void error type */
    using Error = std::conditional_t<HasErrorType, ErrorType, DummyType>;


    /** @brief Destructor */
    inline ~Expected(void) noexcept
    {
        if constexpr (HasErrorType) {
            if (_success)
                _value.~Type();
            else
                _error.~Error();
        } else {
            if (_success)
                _value.~Type();
        }
    }

    /** @brief Copy constructor */
    inline Expected(const Expected &other) noexcept
            requires (std::is_copy_constructible_v<Type> && (!HasErrorType || std::is_copy_constructible_v<Error>))
        : _success(other._success)
    {
        if constexpr (HasErrorType) {
            if (_success)
                new (&_value) Type { other._value };
            else
                new (&_error) Error { other._error };
        } else {
            if (_success)
                new (&_value) Type { other._value };
        }
    }

    /** @brief Move constructor */
    inline Expected(Expected &&other) noexcept
            requires (std::is_move_constructible_v<Type> && (!HasErrorType || std::is_move_constructible_v<Error>))
        : _success(other._success)
    {
        if constexpr (HasErrorType) {
            if (_success)
                new (&_value) Type { std::move(other._value) };
            else
                new (&_error) Error { std::move(other._error) };
        } else {
            if (_success)
                new (&_value) Type { std::move(other._value) };
        }
    }

    /** @brief Default constructor is only usable when !HasErrorType */
    inline Expected(void) noexcept requires (!HasErrorType)
        : _success(false) {}

    /** @brief Copy value constructor */
    explicit inline Expected(const Type &value) noexcept requires (std::is_copy_constructible_v<Type>)
        : _success(true) { new (&_value) Type(value); }


    /** @brief Move value constructor */
    explicit inline Expected(Type &&value) noexcept requires (std::is_move_constructible_v<Type>)
        : _success(true) { new (&_value) Type(std::move(value)); }


    /** @brief Copy error constructor */
    explicit inline Expected(const Error &error) noexcept
            requires (HasErrorType && std::is_copy_constructible_v<Error>)
        : _success(false) { new (&_error) Error(error); }

    /** @brief Move error constructor */
    explicit inline Expected(Error &&error) noexcept
            requires (HasErrorType && std::is_move_constructible_v<Error>)
        : _success(false) { new (&_error) Error(std::move(error)); }


    /** @brief Copy constructor */
    inline Expected &operator=(const Expected &other) noexcept
    {
        this->~Expected();
        new (this) Expected(other);
        return *this;
    }

    /** @brief Move constructor */
    inline Expected &operator=(Expected &&other) noexcept
    {
        this->~Expected();
        new (this) Expected(std::move(other));
        return *this;
    }


    /** @brief Success check */
    [[nodiscard]] inline bool success(void) const noexcept { return _success; }


    /** @brief Access value */
    [[nodiscard]] inline Type *operator->(void) noexcept { return &_value; }

    /** @brief Access constant value */
    [[nodiscard]] inline const Type *operator->(void) const noexcept { return &_value; }

    /** @brief Dereference value */
    [[nodiscard]] inline Type &operator*(void) noexcept { return _value; }

    /** @brief Dereference constant value */
    [[nodiscard]] inline const Type &operator*(void) const noexcept { return _value; }

    /** @brief Dereference value */
    [[nodiscard]] inline Type &value(void) noexcept { return _value; }

    /** @brief Dereference constant value */
    [[nodiscard]] inline const Type &value(void) const noexcept { return _value; }


    /** @brief Dereference error */
    [[nodiscard]] inline Error &error(void) noexcept requires (HasErrorType) { return _error; }

    /** @brief Dereference constant error */
    [[nodiscard]] inline const Error &error(void) const noexcept requires (HasErrorType) { return _error; }

private:
    union {
        DummyType _dummy {};
        Type _value;
        Error _error;
    };
    bool _success;
};