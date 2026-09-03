#pragma once

#include "Error.h"
#include "Result.h"

#include <cstdint>
#include <string>
#include <vector>

namespace space2x::core {

struct NetworkInterface {
    std::string              name{};
    std::string              description{};
    std::string              macAddress{};
    std::vector<std::string> ipv4Addresses{};
    std::vector<std::string> ipv6Addresses{};
    std::string              defaultGateway{};
    bool                     isUp{false};
};

struct ListeningSocket {
    std::string protocol{}; // "tcp", "udp"
    std::string localAddress{};
    uint16_t    localPort{0};
    uint32_t    pid{0};
    std::string processName{};
};

class INetworkManager {
public:
    virtual ~INetworkManager() = default;

    [[nodiscard]] virtual Result<std::vector<NetworkInterface>> getInterfaces() = 0;
    [[nodiscard]] virtual Result<std::vector<ListeningSocket>>  getListeningSockets() = 0;
    [[nodiscard]] virtual Result<bool>                         isPortInUse(uint16_t port, const std::string& protocol = "tcp") = 0;
};

} // namespace space2x::core
