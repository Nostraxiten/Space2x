#include "ProfilesCommand.h"
#include <space2x/core/Engine.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace space2x::cli {

int ProfilesCommand::execute(core::Engine& engine,
                            const std::vector<std::string>& args,
                            bool jsonMode) {
    if (args.empty() || args[0] == "list") {
        auto profiles = engine.profileManager().listProfiles();
        if (jsonMode) {
            nlohmann::json jArr = nlohmann::json::array();
            for (const auto& p : profiles) {
                nlohmann::json svcArr = nlohmann::json::array();
                for (const auto& s : p.services) {
                    svcArr.push_back({
                        {"id", s.serviceId},
                        {"action", s.action},
                        {"autoStart", s.autoStart}
                    });
                }
                jArr.push_back({
                    {"name", p.name},
                    {"version", p.version},
                    {"description", p.description},
                    {"services", svcArr}
                });
            }
            std::cout << jArr.dump(2) << "\n";
            return 0;
        }

        std::cout << "Available Profiles:\n";
        std::cout << std::string(60, '-') << "\n";
        for (const auto& p : profiles) {
            std::cout << "• " << p.name << " (v" << p.version << ")\n";
            std::cout << "  " << p.description << "\n";
            std::cout << "  Services:\n";
            for (const auto& s : p.services) {
                std::cout << "    - " << s.serviceId << " [" << s.action << "]\n";
            }
            std::cout << "\n";
        }
        return 0;
    }

    if (args[0] == "apply") {
        if (args.size() < 2) {
            std::cerr << "Error: Profile name required.\n";
            std::cerr << "Usage: space2x profiles apply <profile-name> [--dry-run]\n";
            return 1;
        }

        std::string profileName = args[1];
        bool dryRun = false;
        if (args.size() >= 3 && args[2] == "--dry-run") {
            dryRun = true;
        }

        auto res = engine.profileManager().applyProfile(profileName, engine.serviceController(), dryRun);
        if (!res.isOk()) {
            std::cerr << "Failed to apply profile: " << res.error().toString() << "\n";
            return 1;
        }

        std::cout << (dryRun ? "Dry-run profile execution:\n" : "Profile applied successfully:\n");
        for (const auto& action : res.value()) {
            std::cout << "  ✓ " << action << "\n";
        }
        return 0;
    }

    std::cerr << "Unknown profiles sub-command: " << args[0] << "\n";
    return 1;
}

} // namespace space2x::cli
