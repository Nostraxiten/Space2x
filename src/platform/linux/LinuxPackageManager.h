#pragma once

#include <space2x/core/IPackageManager.h>

namespace space2x::platform::linux_os {

class LinuxPackageManager : public core::IPackageManager {
public:
    LinuxPackageManager() = default;
    ~LinuxPackageManager() override = default;

    [[nodiscard]] core::Result<std::string>              detectPackageManager() override;
    [[nodiscard]] core::Result<bool>                     isPackageInstalled(const std::string& packageName) override;
    [[nodiscard]] core::Result<std::string>              getInstalledVersion(const std::string& packageName) override;
    [[nodiscard]] core::Result<std::vector<core::PackageInfo>> listInstalledPackages() override;
};

} // namespace space2x::platform::linux_os
