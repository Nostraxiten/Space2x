#pragma once

#include <space2x/core/IPackageManager.h>

namespace space2x::platform::windows {

class WinPackageManager : public core::IPackageManager {
public:
    WinPackageManager() = default;
    ~WinPackageManager() override = default;

    [[nodiscard]] core::Result<std::string>              detectPackageManager() override;
    [[nodiscard]] core::Result<bool>                     isPackageInstalled(const std::string& packageName) override;
    [[nodiscard]] core::Result<std::string>              getInstalledVersion(const std::string& packageName) override;
    [[nodiscard]] core::Result<std::vector<core::PackageInfo>> listInstalledPackages() override;
};

} // namespace space2x::platform::windows
