#pragma once

#include <string_view>

namespace space2x::core {

enum class ServiceState {
    NotInstalled,
    Installed,
    Starting,
    Running,
    Stopping,
    Stopped,
    Degraded,
    Unknown
};

[[nodiscard]] constexpr std::string_view serviceStateToString(ServiceState state) noexcept {
    switch (state) {
        case ServiceState::NotInstalled: return "NotInstalled";
        case ServiceState::Installed:    return "Installed";
        case ServiceState::Starting:     return "Starting";
        case ServiceState::Running:      return "Running";
        case ServiceState::Stopping:     return "Stopping";
        case ServiceState::Stopped:      return "Stopped";
        case ServiceState::Degraded:     return "Degraded";
        case ServiceState::Unknown:      return "Unknown";
    }
    return "Unknown";
}

[[nodiscard]] constexpr ServiceState serviceStateFromString(std::string_view str) noexcept {
    if (str == "NotInstalled") return ServiceState::NotInstalled;
    if (str == "Installed")    return ServiceState::Installed;
    if (str == "Starting")     return ServiceState::Starting;
    if (str == "Running")      return ServiceState::Running;
    if (str == "Stopping")     return ServiceState::Stopping;
    if (str == "Stopped")      return ServiceState::Stopped;
    if (str == "Degraded")     return ServiceState::Degraded;
    return ServiceState::Unknown;
}

} // namespace space2x::core
