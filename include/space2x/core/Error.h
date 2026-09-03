#pragma once

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace space2x::core {

enum class ErrorCode {
    Success = 0,
    ServiceNotFound,
    ServiceNotRunning,
    ServiceAlreadyRunning,
    ServiceStartFailed,
    ServiceStopFailed,
    PermissionDenied,
    PortInUse,
    ConfigInvalid,
    ConfigFileNotFound,
    PlatformNotSupported,
    NetworkUnavailable,
    ProcessNotFound,
    Timeout,
    IoError,
    ParseError,
    NotImplemented,
    Unknown
};

[[nodiscard]] constexpr std::string_view errorCodeToString(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Success:               return "Success";
        case ErrorCode::ServiceNotFound:       return "ServiceNotFound";
        case ErrorCode::ServiceNotRunning:      return "ServiceNotRunning";
        case ErrorCode::ServiceAlreadyRunning:  return "ServiceAlreadyRunning";
        case ErrorCode::ServiceStartFailed:     return "ServiceStartFailed";
        case ErrorCode::ServiceStopFailed:      return "ServiceStopFailed";
        case ErrorCode::PermissionDenied:       return "PermissionDenied";
        case ErrorCode::PortInUse:             return "PortInUse";
        case ErrorCode::ConfigInvalid:          return "ConfigInvalid";
        case ErrorCode::ConfigFileNotFound:     return "ConfigFileNotFound";
        case ErrorCode::PlatformNotSupported:   return "PlatformNotSupported";
        case ErrorCode::NetworkUnavailable:     return "NetworkUnavailable";
        case ErrorCode::ProcessNotFound:        return "ProcessNotFound";
        case ErrorCode::Timeout:               return "Timeout";
        case ErrorCode::IoError:               return "IoError";
        case ErrorCode::ParseError:             return "ParseError";
        case ErrorCode::NotImplemented:         return "NotImplemented";
        case ErrorCode::Unknown:               return "Unknown";
    }
    return "Unknown";
}

/**
 * Structured diagnostic error object.
 */
struct Error {
    ErrorCode                code{ErrorCode::Unknown};
    std::string              message{};
    std::string              detail{};
    std::string              suggestion{};
    std::shared_ptr<Error>   cause{nullptr};

    static Error make(ErrorCode code,
                      std::string message,
                      std::string detail = "",
                      std::string suggestion = "",
                      std::shared_ptr<Error> cause = nullptr) {
        Error err;
        err.code = code;
        err.message = std::move(message);
        err.detail = std::move(detail);
        err.suggestion = std::move(suggestion);
        err.cause = std::move(cause);
        return err;
    }

    [[nodiscard]] std::string toString() const {
        std::ostringstream oss;
        oss << "[" << errorCodeToString(code) << "] " << message;
        if (!detail.empty()) {
            oss << "\n  Detail: " << detail;
        }
        if (!suggestion.empty()) {
            oss << "\n  Suggestion: " << suggestion;
        }
        if (cause) {
            oss << "\n  Caused by:\n" << cause->toString();
        }
        return oss.str();
    }
};

} // namespace space2x::core
