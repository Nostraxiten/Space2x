#include "ProfileManager.h"
#include "ServiceController.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace space2x::core {

Result<ServiceProfile> ProfileManager::loadProfileFromFile(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        return Result<ServiceProfile>::err(Error::make(
            ErrorCode::ConfigFileNotFound,
            "Cannot open profile file: " + jsonFilePath
        ));
    }

    try {
        nlohmann::json j;
        file >> j;

        ServiceProfile profile;
        if (j.contains("name")) profile.name = j["name"].get<std::string>();
        if (j.contains("version")) profile.version = j["version"].get<std::string>();
        if (j.contains("description")) profile.description = j["description"].get<std::string>();

        if (j.contains("services") && j["services"].is_array()) {
            for (const auto& item : j["services"]) {
                ProfileServiceAction psa;
                if (item.contains("id")) psa.serviceId = item["id"].get<std::string>();
                if (item.contains("action")) psa.action = item["action"].get<std::string>();
                if (item.contains("autoStart")) psa.autoStart = item["autoStart"].get<bool>();
                profile.services.push_back(std::move(psa));
            }
        }

        registerProfile(profile);
        return Result<ServiceProfile>::ok(std::move(profile));
    } catch (const std::exception& e) {
        return Result<ServiceProfile>::err(Error::make(
            ErrorCode::ParseError,
            "Failed to parse profile JSON: " + std::string(e.what())
        ));
    }
}

void ProfileManager::registerProfile(ServiceProfile profile) {
    m_profiles[profile.name] = std::move(profile);
}

Result<ServiceProfile> ProfileManager::getProfile(const std::string& name) const {
    auto it = m_profiles.find(name);
    if (it != m_profiles.end()) {
        return Result<ServiceProfile>::ok(it->second);
    }
    return Result<ServiceProfile>::err(Error::make(
        ErrorCode::ServiceNotFound,
        "Profile not found: " + name
    ));
}

std::vector<ServiceProfile> ProfileManager::listProfiles() const {
    std::vector<ServiceProfile> result;
    result.reserve(m_profiles.size());
    for (const auto& [_, p] : m_profiles) {
        result.push_back(p);
    }
    return result;
}

Result<std::vector<std::string>> ProfileManager::applyProfile(const std::string& profileName,
                                                              ServiceController& controller,
                                                              bool dryRun) {
    auto profileRes = getProfile(profileName);
    if (!profileRes.isOk()) {
        return Result<std::vector<std::string>>::err(profileRes.error());
    }

    const auto& profile = profileRes.value();
    std::vector<std::string> appliedActions;

    for (const auto& svc : profile.services) {
        std::string summary = svc.serviceId + ": " + svc.action;
        if (dryRun) {
            appliedActions.push_back("[DryRun] " + summary);
            continue;
        }

        if (svc.action == "ensure_running") {
            auto startRes = controller.startService(svc.serviceId);
            if (!startRes.isOk() && startRes.error().code != ErrorCode::ServiceAlreadyRunning) {
                return Result<std::vector<std::string>>::err(startRes.error());
            }
            appliedActions.push_back("Started " + svc.serviceId);
        } else if (svc.action == "ensure_stopped") {
            auto stopRes = controller.stopService(svc.serviceId);
            if (!stopRes.isOk() && stopRes.error().code != ErrorCode::ServiceNotRunning) {
                return Result<std::vector<std::string>>::err(stopRes.error());
            }
            appliedActions.push_back("Stopped " + svc.serviceId);
        } else if (svc.action == "restart") {
            auto rstRes = controller.restartService(svc.serviceId);
            if (!rstRes.isOk()) {
                return Result<std::vector<std::string>>::err(rstRes.error());
            }
            appliedActions.push_back("Restarted " + svc.serviceId);
        }

        if (svc.autoStart) {
            auto autoRes = controller.setAutoStart(svc.serviceId, true);
            if (autoRes.isOk()) {
                appliedActions.push_back("Enabled auto-start for " + svc.serviceId);
            }
        }
    }

    return Result<std::vector<std::string>>::ok(std::move(appliedActions));
}

} // namespace space2x::core
