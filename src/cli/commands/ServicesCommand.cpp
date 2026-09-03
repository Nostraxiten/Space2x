#include "ServicesCommand.h"
#include <space2x/core/Engine.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

namespace space2x::cli {

int ServicesCommand::execute(core::Engine& engine,
                            const std::vector<std::string>& args,
                            bool jsonMode) {
    if (args.empty() || args[0] == "list") {
        auto res = engine.serviceController().listServices();
        if (!res.isOk()) {
            std::cerr << res.error().toString() << "\n";
            return 1;
        }

        if (jsonMode) {
            nlohmann::json jArr = nlohmann::json::array();
            for (const auto& s : res.value()) {
                jArr.push_back({
                    {"id", s.id},
                    {"name", s.displayName},
                    {"category", s.category},
                    {"state", std::string(core::serviceStateToString(s.state))},
                    {"port", s.port}
                });
            }
            std::cout << jArr.dump(2) << "\n";
            return 0;
        }

        std::cout << std::left
                  << std::setw(15) << "SERVICE ID"
                  << std::setw(28) << "NAME"
                  << std::setw(15) << "CATEGORY"
                  << std::setw(15) << "STATUS"
                  << std::setw(8)  << "PORT"
                  << "\n";
        std::cout << std::string(80, '-') << "\n";

        for (const auto& s : res.value()) {
            std::cout << std::left
                      << std::setw(15) << s.id
                      << std::setw(28) << s.displayName
                      << std::setw(15) << s.category
                      << std::setw(15) << core::serviceStateToString(s.state)
                      << std::setw(8)  << (s.port > 0 ? std::to_string(s.port) : "-")
                      << "\n";
        }
        return 0;
    }

    const std::string& action = args[0];

    if (args.size() < 2) {
        std::cerr << "Error: Action '" << action << "' requires a service id argument.\n";
        std::cerr << "Usage: space2x services " << action << " <service-id>\n";
        return 1;
    }

    const std::string& serviceId = args[1];

    if (action == "start") {
        auto res = engine.serviceController().startService(serviceId);
        if (!res.isOk()) {
            std::cerr << res.error().toString() << "\n";
            return 1;
        }
        std::cout << "Service '" << serviceId << "' started successfully.\n";
        return 0;
    }

    if (action == "stop") {
        auto res = engine.serviceController().stopService(serviceId);
        if (!res.isOk()) {
            std::cerr << res.error().toString() << "\n";
            return 1;
        }
        std::cout << "Service '" << serviceId << "' stopped successfully.\n";
        return 0;
    }

    if (action == "restart") {
        auto res = engine.serviceController().restartService(serviceId);
        if (!res.isOk()) {
            std::cerr << res.error().toString() << "\n";
            return 1;
        }
        std::cout << "Service '" << serviceId << "' restarted successfully.\n";
        return 0;
    }

    if (action == "status") {
        auto res = engine.serviceController().getServiceState(serviceId);
        if (!res.isOk()) {
            std::cerr << res.error().toString() << "\n";
            return 1;
        }
        if (jsonMode) {
            nlohmann::json j = {
                {"id", serviceId},
                {"state", std::string(core::serviceStateToString(res.value()))}
            };
            std::cout << j.dump(2) << "\n";
        } else {
            std::cout << "Service '" << serviceId << "' status: " << core::serviceStateToString(res.value()) << "\n";
        }
        return 0;
    }

    if (action == "health") {
        auto res = engine.serviceController().performHealthCheck(serviceId);
        if (!res.isOk()) {
            std::cerr << "Health check FAILED for '" << serviceId << "':\n" << res.error().toString() << "\n";
            return 1;
        }
        std::cout << "Health check PASSED for service '" << serviceId << "'.\n";
        return 0;
    }

    std::cerr << "Unknown services sub-command: " << action << "\n";
    return 1;
}

} // namespace space2x::cli
