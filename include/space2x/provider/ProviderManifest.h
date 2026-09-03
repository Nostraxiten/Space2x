#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace space2x::provider {

struct HealthCheckConfig {
    std::string type{"tcp"}; // "tcp", "http", "process", "none"
    uint16_t    port{0};
    std::string path{};
    uint32_t    timeoutMs{2000};
};

struct ProviderManifest {
    std::string                                  id{};
    std::string                                  displayName{};
    std::string                                  version{"1.0.0"};
    std::string                                  category{"general"};
    std::string                                  description{};
    std::vector<std::string>                     supportedPlatforms{};
    std::vector<std::string>                     supportedArchitectures{};
    uint16_t                                     defaultPort{0};
    std::map<std::string, std::string>           serviceNames{};
    std::map<std::string, std::vector<std::string>> configPaths{};
    std::map<std::string, std::vector<std::string>> logPaths{};
    HealthCheckConfig                            healthCheck{};

    [[nodiscard]] bool isPlatformSupported(const std::string& platform) const {
        for (const auto& p : supportedPlatforms) {
            if (p == platform || p == "all") return true;
        }
        return false;
    }

    [[nodiscard]] std::string serviceNameForPlatform(const std::string& platform) const {
        auto it = serviceNames.find(platform);
        if (it != serviceNames.end()) {
            return it->second;
        }
        return id;
    }

    [[nodiscard]] std::string serviceNameForCurrentPlatform() const {
#if defined(_WIN32)
        return serviceNameForPlatform("windows");
#elif defined(__linux__)
        return serviceNameForPlatform("linux");
#elif defined(__APPLE__)
        return serviceNameForPlatform("macos");
#else
        return id;
#endif
    }

    static ProviderManifest fromJson(const nlohmann::json& j) {
        ProviderManifest m;
        if (j.contains("id")) m.id = j["id"].get<std::string>();
        if (j.contains("displayName")) m.displayName = j["displayName"].get<std::string>();
        if (j.contains("version")) m.version = j["version"].get<std::string>();
        if (j.contains("category")) m.category = j["category"].get<std::string>();
        if (j.contains("description")) m.description = j["description"].get<std::string>();
        if (j.contains("supportedPlatforms")) m.supportedPlatforms = j["supportedPlatforms"].get<std::vector<std::string>>();
        if (j.contains("supportedArchitectures")) m.supportedArchitectures = j["supportedArchitectures"].get<std::vector<std::string>>();
        if (j.contains("defaultPort")) m.defaultPort = j["defaultPort"].get<uint16_t>();

        if (j.contains("serviceNames") && j["serviceNames"].is_object()) {
            for (auto& [k, v] : j["serviceNames"].items()) {
                m.serviceNames[k] = v.get<std::string>();
            }
        }

        if (j.contains("configPaths") && j["configPaths"].is_object()) {
            for (auto& [k, v] : j["configPaths"].items()) {
                if (v.is_array()) {
                    m.configPaths[k] = v.get<std::vector<std::string>>();
                }
            }
        }

        if (j.contains("logPaths") && j["logPaths"].is_object()) {
            for (auto& [k, v] : j["logPaths"].items()) {
                if (v.is_array()) {
                    m.logPaths[k] = v.get<std::vector<std::string>>();
                }
            }
        }

        if (j.contains("healthCheck") && j["healthCheck"].is_object()) {
            const auto& hc = j["healthCheck"];
            if (hc.contains("type")) m.healthCheck.type = hc["type"].get<std::string>();
            if (hc.contains("port")) m.healthCheck.port = hc["port"].get<uint16_t>();
            if (hc.contains("path")) m.healthCheck.path = hc["path"].get<std::string>();
            if (hc.contains("timeoutMs")) m.healthCheck.timeoutMs = hc["timeoutMs"].get<uint32_t>();
        }

        return m;
    }

    [[nodiscard]] nlohmann::json toJson() const {
        nlohmann::json j;
        j["id"] = id;
        j["displayName"] = displayName;
        j["version"] = version;
        j["category"] = category;
        j["description"] = description;
        j["supportedPlatforms"] = supportedPlatforms;
        j["supportedArchitectures"] = supportedArchitectures;
        j["defaultPort"] = defaultPort;
        j["serviceNames"] = serviceNames;
        j["configPaths"] = configPaths;
        j["logPaths"] = logPaths;
        j["healthCheck"] = {
            {"type", healthCheck.type},
            {"port", healthCheck.port},
            {"path", healthCheck.path},
            {"timeoutMs", healthCheck.timeoutMs}
        };
        return j;
    }
};

} // namespace space2x::provider
