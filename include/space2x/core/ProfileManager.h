#pragma once

#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <map>
#include <string>
#include <vector>

namespace space2x::core {

class ServiceController;

struct ProfileServiceAction {
    std::string serviceId{};
    std::string action{"ensure_running"}; // "ensure_running", "ensure_stopped", "restart", "ignore"
    bool        autoStart{false};
};

struct ServiceProfile {
    std::string                       name{};
    std::string                       version{"1.0.0"};
    std::string                       description{};
    std::vector<ProfileServiceAction> services{};
};

class ProfileManager {
public:
    ProfileManager() = default;
    ~ProfileManager() = default;

    Result<ServiceProfile> loadProfileFromFile(const std::string& jsonFilePath);
    void registerProfile(ServiceProfile profile);

    [[nodiscard]] Result<ServiceProfile> getProfile(const std::string& name) const;
    [[nodiscard]] std::vector<ServiceProfile> listProfiles() const;

    Result<std::vector<std::string>> applyProfile(const std::string& profileName,
                                                  ServiceController& controller,
                                                  bool dryRun = false);

private:
    std::map<std::string, ServiceProfile> m_profiles;
};

} // namespace space2x::core
