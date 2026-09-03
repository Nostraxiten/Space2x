#pragma once

#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <map>
#include <string>
#include <vector>

namespace space2x::core {

struct ServiceVersionInfo {
    std::string              serviceId{};
    std::string              installedVersion{};
    std::string              recommendedVersion{};
    std::vector<std::string> availableVersions{};
    std::vector<std::string> deprecatedVersions{};
};

class VersionStore {
public:
    VersionStore() = default;
    ~VersionStore() = default;

    void registerServiceVersion(const ServiceVersionInfo& info);
    [[nodiscard]] Result<ServiceVersionInfo> getVersionInfo(const std::string& serviceId) const;
    [[nodiscard]] std::vector<ServiceVersionInfo> all() const;

private:
    std::map<std::string, ServiceVersionInfo> m_store;
};

} // namespace space2x::core
