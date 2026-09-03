#include "NetworkCommand.h"
#include <space2x/core/Engine.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

namespace space2x::cli {

int NetworkCommand::execute(core::Engine& engine,
                            const std::vector<std::string>& args,
                            bool jsonMode) {
    if (args.empty() || args[0] == "status" || args[0] == "interfaces") {
        auto ifacesRes = engine.networkManager().getInterfaces();
        auto socketsRes = engine.networkManager().getListeningSockets();

        if (jsonMode) {
            nlohmann::json j;
            nlohmann::json ifaceArr = nlohmann::json::array();
            if (ifacesRes.isOk()) {
                for (const auto& iface : ifacesRes.value()) {
                    ifaceArr.push_back({
                        {"name", iface.name},
                        {"description", iface.description},
                        {"isUp", iface.isUp},
                        {"macAddress", iface.macAddress},
                        {"ipv4", iface.ipv4Addresses},
                        {"ipv6", iface.ipv6Addresses},
                        {"gateway", iface.defaultGateway}
                    });
                }
            }
            j["interfaces"] = ifaceArr;

            nlohmann::json sockArr = nlohmann::json::array();
            if (socketsRes.isOk()) {
                for (const auto& s : socketsRes.value()) {
                    sockArr.push_back({
                        {"protocol", s.protocol},
                        {"localAddress", s.localAddress},
                        {"port", s.localPort},
                        {"pid", s.pid}
                    });
                }
            }
            j["listeningSockets"] = sockArr;

            std::cout << j.dump(2) << "\n";
            return 0;
        }

        std::cout << "Network Interfaces:\n";
        std::cout << std::string(70, '-') << "\n";
        if (ifacesRes.isOk()) {
            for (const auto& iface : ifacesRes.value()) {
                std::cout << "• " << iface.description << " [" << (iface.isUp ? "UP" : "DOWN") << "]\n";
                std::cout << "  MAC: " << iface.macAddress << "\n";
                for (const auto& ip : iface.ipv4Addresses) {
                    std::cout << "  IPv4: " << ip << "\n";
                }
                if (!iface.defaultGateway.empty()) {
                    std::cout << "  Gateway: " << iface.defaultGateway << "\n";
                }
            }
        }
        std::cout << "\nListening Ports (TCP):\n";
        std::cout << std::string(70, '-') << "\n";
        if (socketsRes.isOk()) {
            std::cout << std::left
                      << std::setw(10) << "PROTO"
                      << std::setw(22) << "LOCAL ADDRESS"
                      << std::setw(10) << "PORT"
                      << std::setw(10) << "PID"
                      << "\n";
            for (const auto& s : socketsRes.value()) {
                std::cout << std::left
                          << std::setw(10) << s.protocol
                          << std::setw(22) << s.localAddress
                          << std::setw(10) << s.localPort
                          << std::setw(10) << s.pid
                          << "\n";
            }
        }
        return 0;
    }

    std::cerr << "Unknown network sub-command: " << args[0] << "\n";
    return 1;
}

} // namespace space2x::cli
