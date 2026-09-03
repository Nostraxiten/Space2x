#include "LinuxServiceManager.h"

#if defined(__linux__)
#include <cstdlib>
#include <array>
#include <memory>
#include <sstream>
#include <iostream>

namespace space2x::platform::linux_os {

namespace {

int runSystemctlCommand(const std::string& action, const std::string& serviceName) {
    std::string cmd = "systemctl " + action + " " + serviceName + " > /dev/null 2>&1";
    return std::system(cmd.c_str());
}

std::string getCommandOutput(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

} // anonymous namespace

core::Result<std::vector<core::ServiceInfo>> LinuxServiceManager::listServices() {
    std::string output = getCommandOutput("systemctl list-units --type=service --all --no-pager --plain --no-legend");
    std::vector<core::ServiceInfo> services;
    std::istringstream iss(output);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        std::istringstream lineStream(line);
        std::string unit, load, active, sub;
        lineStream >> unit >> load >> active >> sub;

        if (unit.empty()) continue;

        core::ServiceInfo info;
        info.id = unit;
        info.displayName = unit;
        if (active == "active") {
            info.state = core::ServiceState::Running;
        } else if (active == "inactive") {
            info.state = core::ServiceState::Stopped;
        } else if (active == "failed") {
            info.state = core::ServiceState::Degraded;
        } else {
            info.state = core::ServiceState::Unknown;
        }

        services.push_back(std::move(info));
    }

    return core::Result<std::vector<core::ServiceInfo>>::ok(std::move(services));
}

core::Result<core::ServiceState> LinuxServiceManager::getState(const std::string& serviceId) {
    std::string output = getCommandOutput("systemctl is-active " + serviceId);
    while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
        output.pop_back();
    }

    if (output == "active") {
        return core::Result<core::ServiceState>::ok(core::ServiceState::Running);
    }
    if (output == "inactive") {
        return core::Result<core::ServiceState>::ok(core::ServiceState::Stopped);
    }
    if (output == "failed") {
        return core::Result<core::ServiceState>::ok(core::ServiceState::Degraded);
    }
    if (output == "activating") {
        return core::Result<core::ServiceState>::ok(core::ServiceState::Starting);
    }
    if (output == "deactivating") {
        return core::Result<core::ServiceState>::ok(core::ServiceState::Stopping);
    }

    // Check if unit exists
    std::string unitStatus = getCommandOutput("systemctl status " + serviceId + " 2>&1");
    if (unitStatus.find("could not be found") != std::string::npos ||
        unitStatus.find("not found") != std::string::npos) {
        return core::Result<core::ServiceState>::ok(core::ServiceState::NotInstalled);
    }

    return core::Result<core::ServiceState>::ok(core::ServiceState::Unknown);
}

core::Result<void> LinuxServiceManager::start(const std::string& serviceId) {
    int ret = runSystemctlCommand("start", serviceId);
    if (ret != 0) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceStartFailed,
            "Failed to start systemd service '" + serviceId + "'.",
            "systemctl start exited with code " + std::to_string(ret),
            "Ensure Space2X or user has sufficient privileges (e.g. via polkit or sudo)."
        ));
    }
    return core::Result<void>::ok();
}

core::Result<void> LinuxServiceManager::stop(const std::string& serviceId) {
    int ret = runSystemctlCommand("stop", serviceId);
    if (ret != 0) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceStopFailed,
            "Failed to stop systemd service '" + serviceId + "'.",
            "systemctl stop exited with code " + std::to_string(ret),
            "Ensure Space2X or user has sufficient privileges (e.g. via polkit or sudo)."
        ));
    }
    return core::Result<void>::ok();
}

core::Result<void> LinuxServiceManager::restart(const std::string& serviceId) {
    int ret = runSystemctlCommand("restart", serviceId);
    if (ret != 0) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceStartFailed,
            "Failed to restart systemd service '" + serviceId + "'.",
            "systemctl restart exited with code " + std::to_string(ret),
            "Check systemctl status " + serviceId + " for service failure logs."
        ));
    }
    return core::Result<void>::ok();
}

core::Result<void> LinuxServiceManager::setAutoStart(const std::string& serviceId, bool enabled) {
    std::string action = enabled ? "enable" : "disable";
    int ret = runSystemctlCommand(action, serviceId);
    if (ret != 0) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::PermissionDenied,
            "Failed to " + action + " auto-start for service '" + serviceId + "'.",
            "systemctl " + action + " exited with code " + std::to_string(ret)
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::platform::linux_os

#else

namespace space2x::platform::linux_os {

core::Result<std::vector<core::ServiceInfo>> LinuxServiceManager::listServices() {
    return core::Result<std::vector<core::ServiceInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

core::Result<core::ServiceState> LinuxServiceManager::getState(const std::string&) {
    return core::Result<core::ServiceState>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

core::Result<void> LinuxServiceManager::start(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

core::Result<void> LinuxServiceManager::stop(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

core::Result<void> LinuxServiceManager::restart(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

core::Result<void> LinuxServiceManager::setAutoStart(const std::string&, bool) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxServiceManager is only supported on Linux.")
    );
}

} // namespace space2x::platform::linux_os

#endif
