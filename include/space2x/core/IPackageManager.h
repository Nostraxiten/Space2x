#pragma once

#include "Error.h"
#include "Result.h"

#include <string>
#include <vector>

namespace space2x::core {

struct PackageInfo {
    std::string name{};
    std::string version{};
    bool        isInstalled{false};
    std::string source{};
};

class IPackageManager {
public:
    virtual ~IPackageManager() = default;

    [[nodiscard]] virtual Result<std::string>              detectPackageManager() = 0;
    [[nodiscard]] virtual Result<bool>                     isPackageInstalled(const std::string& packageName) = 0;
    [[nodiscard]] virtual Result<std::string>              getInstalledVersion(const std::string& packageName) = 0;
    [[nodiscard]] virtual Result<std::vector<PackageInfo>> listInstalledPackages() = 0;
};

} // namespace space2x::core
