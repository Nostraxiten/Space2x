#include "WinPackageManager.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <memory>

namespace space2x::platform::windows {

namespace {

bool commandExists(const std::string& cmd) {
    std::string checkCmd = "where.exe " + cmd + " >nul 2>&1";
    int res = std::system(checkCmd.c_str());
    return res == 0;
}

} // anonymous namespace

core::Result<std::string> WinPackageManager::detectPackageManager() {
    if (commandExists("winget")) {
        return core::Result<std::string>::ok("winget");
    }
    if (commandExists("choco")) {
        return core::Result<std::string>::ok("choco");
    }
    if (commandExists("scoop")) {
        return core::Result<std::string>::ok("scoop");
    }
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::Unknown, "No supported Windows package manager (winget/choco/scoop) detected in PATH.")
    );
}

core::Result<bool> WinPackageManager::isPackageInstalled(const std::string& packageName) {
    auto pmRes = detectPackageManager();
    if (!pmRes.isOk()) {
        return core::Result<bool>::err(pmRes.error());
    }

    const auto& pm = pmRes.value();
    if (pm == "winget") {
        std::string cmd = "winget list --exact --id \"" + packageName + "\" >nul 2>&1";
        int res = std::system(cmd.c_str());
        return core::Result<bool>::ok(res == 0);
    }

    return core::Result<bool>::ok(false);
}

core::Result<std::string> WinPackageManager::getInstalledVersion(const std::string& packageName) {
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

core::Result<std::vector<core::PackageInfo>> WinPackageManager::listInstalledPackages() {
    std::vector<core::PackageInfo> pkgs;
    return core::Result<std::vector<core::PackageInfo>>::ok(std::move(pkgs));
}

} // namespace space2x::platform::windows

#else

namespace space2x::platform::windows {

core::Result<std::string> WinPackageManager::detectPackageManager() {
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinPackageManager is only supported on Windows.")
    );
}

core::Result<bool> WinPackageManager::isPackageInstalled(const std::string&) {
    return core::Result<bool>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinPackageManager is only supported on Windows.")
    );
}

core::Result<std::string> WinPackageManager::getInstalledVersion(const std::string&) {
    return core::Result<std::string>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinPackageManager is only supported on Windows.")
    );
}

core::Result<std::vector<core::PackageInfo>> WinPackageManager::listInstalledPackages() {
    return core::Result<std::vector<core::PackageInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinPackageManager is only supported on Windows.")
    );
}

} // namespace space2x::platform::windows

#endif
