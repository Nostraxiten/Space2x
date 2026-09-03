#pragma once

#include <cassert>
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace space2x::core {

// Forward declaration
struct Error;

/**
 * Monadic Result<T, E> container representing either success with a value of type T
 * or failure with an error of type E (defaults to Error).
 */
template <typename T, typename E = Error>
class [[nodiscard]] Result {
public:
    using value_type = T;
    using error_type = E;

    // Constructors
    static Result ok(T value) {
        return Result(std::in_place_index<0>, std::move(value));
    }

    static Result err(E error) {
        return Result(std::in_place_index<1>, std::move(error));
    }

    // State observers
    [[nodiscard]] constexpr bool isOk() const noexcept {
        return m_storage.index() == 0;
    }

    [[nodiscard]] constexpr bool isErr() const noexcept {
        return m_storage.index() == 1;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return isOk();
    }

    // Value accessors
    [[nodiscard]] const T& value() const & {
        assert(isOk() && "Attempted to access value() on an err Result");
        return std::get<0>(m_storage);
    }

    [[nodiscard]] T& value() & {
        assert(isOk() && "Attempted to access value() on an err Result");
        return std::get<0>(m_storage);
    }

    [[nodiscard]] T&& value() && {
        assert(isOk() && "Attempted to access value() on an err Result");
        return std::get<0>(std::move(m_storage));
    }

    [[nodiscard]] const E& error() const & {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return std::get<1>(m_storage);
    }

    [[nodiscard]] E& error() & {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return std::get<1>(m_storage);
    }

    [[nodiscard]] E&& error() && {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return std::get<1>(std::move(m_storage));
    }

    [[nodiscard]] T valueOr(T defaultValue) const & {
        if (isOk()) {
            return std::get<0>(m_storage);
        }
        return defaultValue;
    }

    [[nodiscard]] T valueOr(T defaultValue) && {
        if (isOk()) {
            return std::get<0>(std::move(m_storage));
        }
        return defaultValue;
    }

    // Monadic combinators
    template <typename F>
    auto map(F&& func) const & -> Result<std::invoke_result_t<F, const T&>, E> {
        using ReturnType = std::invoke_result_t<F, const T&>;
        if (isOk()) {
            return Result<ReturnType, E>::ok(std::invoke(std::forward<F>(func), value()));
        }
        return Result<ReturnType, E>::err(error());
    }

    template <typename F>
    auto map(F&& func) && -> Result<std::invoke_result_t<F, T&&>, E> {
        using ReturnType = std::invoke_result_t<F, T&&>;
        if (isOk()) {
            return Result<ReturnType, E>::ok(std::invoke(std::forward<F>(func), std::move(value())));
        }
        return Result<ReturnType, E>::err(std::move(error()));
    }

    template <typename F>
    auto flatMap(F&& func) const & -> std::invoke_result_t<F, const T&> {
        using ReturnType = std::invoke_result_t<F, const T&>;
        if (isOk()) {
            return std::invoke(std::forward<F>(func), value());
        }
        return ReturnType::err(error());
    }

    template <typename F>
    auto flatMap(F&& func) && -> std::invoke_result_t<F, T&&> {
        using ReturnType = std::invoke_result_t<F, T&&>;
        if (isOk()) {
            return std::invoke(std::forward<F>(func), std::move(value()));
        }
        return ReturnType::err(std::move(error()));
    }

    template <typename F>
    auto mapError(F&& func) const & -> Result<T, std::invoke_result_t<F, const E&>> {
        using NewErrorType = std::invoke_result_t<F, const E&>;
        if (isErr()) {
            return Result<T, NewErrorType>::err(std::invoke(std::forward<F>(func), error()));
        }
        return Result<T, NewErrorType>::ok(value());
    }

private:
    template <size_t I, typename Arg>
    constexpr Result(std::in_place_index_t<I> index, Arg&& arg)
        : m_storage(index, std::forward<Arg>(arg)) {}

    std::variant<T, E> m_storage;
};

/**
 * Void specialization of Result<void, E>
 */
template <typename E>
class [[nodiscard]] Result<void, E> {
public:
    using value_type = void;
    using error_type = E;

    static Result ok() {
        return Result(true, E{});
    }

    static Result err(E error) {
        return Result(false, std::move(error));
    }

    [[nodiscard]] constexpr bool isOk() const noexcept {
        return m_isOk;
    }

    [[nodiscard]] constexpr bool isErr() const noexcept {
        return !m_isOk;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return m_isOk;
    }

    [[nodiscard]] const E& error() const & {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return m_error;
    }

    [[nodiscard]] E& error() & {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return m_error;
    }

    [[nodiscard]] E&& error() && {
        assert(isErr() && "Attempted to access error() on an ok Result");
        return std::move(m_error);
    }

    template <typename F>
    auto map(F&& func) const -> Result<std::invoke_result_t<F>, E> {
        using ReturnType = std::invoke_result_t<F>;
        if (isOk()) {
            if constexpr (std::is_void_v<ReturnType>) {
                std::invoke(std::forward<F>(func));
                return Result<void, E>::ok();
            } else {
                return Result<ReturnType, E>::ok(std::invoke(std::forward<F>(func)));
            }
        }
        return Result<ReturnType, E>::err(m_error);
    }

    template <typename F>
    auto flatMap(F&& func) const -> std::invoke_result_t<F> {
        using ReturnType = std::invoke_result_t<F>;
        if (isOk()) {
            return std::invoke(std::forward<F>(func));
        }
        return ReturnType::err(m_error);
    }

    template <typename F>
    auto mapError(F&& func) const & -> Result<void, std::invoke_result_t<F, const E&>> {
        using NewErrorType = std::invoke_result_t<F, const E&>;
        if (isErr()) {
            return Result<void, NewErrorType>::err(std::invoke(std::forward<F>(func), m_error));
        }
        return Result<void, NewErrorType>::ok();
    }

private:
    constexpr Result(bool isOk, E error)
        : m_isOk(isOk), m_error(std::move(error)) {}

    bool m_isOk{true};
    E m_error{};
};

} // namespace space2x::core
