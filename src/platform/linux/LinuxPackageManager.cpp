#include "LinuxPackageManager.h"

#if defined(__linux__)
#include <cstdlib>
#include <array>
#include <memory>

namespace space2x::platform::linux_os {

namespace {

bool commandExists(const std::string& cmd) {
    std::string checkCmd = "which " + cmd + " > /dev/null 2>&1";
    int res = std::system(checkCmd.c_str());
    return res == 0;
}

} // anonymous namespace

core::Result<std::string> LinuxPackageManager::detectPackageManager() {
    if (commandExists("apt-get") || commandExists("dpkg")) {
        return core::Result<std::string>::ok("apt");
    }
    if (commandExists("dnf")) {
        return core::Result<std::string>::ok("dnf");
    }
    if (commandExists("pacman")) {
        return core::Result<std::string>::ok("pacman");
    }
    if (commandExists("zypper")) {
        return core::Result<std::string>::ok("zypper");
    }
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::Unknown, "No supported Linux package manager detected.")
    );
}

core::Result<bool> LinuxPackageManager::isPackageInstalled(const std::string& packageName) {
    auto pmRes = detectPackageManager();
    if (!pmRes.isOk()) {
        return core::Result<bool>::err(pmRes.error());
    }

    const auto& pm = pmRes.value();
    if (pm == "apt") {
        std::string cmd = "dpkg -s " + packageName + " > /dev/null 2>&1";
        int res = std::system(cmd.c_str());
        return core::Result<bool>::ok(res == 0);
    }
    if (pm == "pacman") {
        std::string cmd = "pacman -Q " + packageName + " > /dev/null 2>&1";
        int res = std::system(cmd.c_str());
        return core::Result<bool>::ok(res == 0);
    }
    if (pm == "dnf") {
        std::string cmd = "rpm -q " + packageName + " > /dev/null 2>&1";
        int res = std::system(cmd.c_str());
        return core::Result<bool>::ok(res == 0);
    }

    return core::Result<bool>::ok(false);
}

core::Result<std::string> LinuxPackageManager::getInstalledVersion(const std::string& packageName) {
    auto installed = isPackageInstalled(packageName);
    if (!installed.isOk()) {
        return core::Result<std::string>::err(installed.error());
    }
    if (!installed.value()) {
        return core::Result<std::string>::err(
            core::Error::make(core::ErrorCode::ServiceNotFound, "Package '" + packageName + "' is not installed.")
        );
    }
    return core::Result<std::string>::ok("detected");
}

core::Result<std::vector<core::PackageInfo>> LinuxPackageManager::listInstalledPackages() {
    std::vector<core::PackageInfo> pkgs;
    return core::Result<std::vector<core::PackageInfo>>::ok(std::move(pkgs));
}

} // namespace space2x::platform::linux_os

#else

namespace space2x::platform::linux_os {

core::Result<std::string> LinuxPackageManager::detectPackageManager() {
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxPackageManager is only supported on Linux.")
    );
}

core::Result<bool> LinuxPackageManager::isPackageInstalled(const std::string&) {
    return core::Result<bool>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxPackageManager is only supported on Linux.")
    );
}

core::Result<std::string> LinuxPackageManager::getInstalledVersion(const std::string&) {
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxPackageManager is only supported on Linux.")
    );
}

core::Result<std::vector<core::PackageInfo>> LinuxPackageManager::listInstalledPackages() {
    return core::Result<std::vector<core::PackageInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxPackageManager is only supported on Linux.")
    );
}

} // namespace space2x::platform::linux_os

#endif
