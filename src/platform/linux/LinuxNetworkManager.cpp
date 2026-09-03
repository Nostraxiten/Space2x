#include "LinuxNetworkManager.h"

#if defined(__linux__)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>

namespace space2x::platform::linux_os {

core::Result<std::vector<core::NetworkInterface>> LinuxNetworkManager::getInterfaces() {
    struct ifaddrs* ifaddr = nullptr;
    if (getifaddrs(&ifaddr) == -1) {
        return core::Result<std::vector<core::NetworkInterface>>::err(
            core::Error::make(core::ErrorCode::NetworkUnavailable, "Failed to query network interfaces via getifaddrs.")
        );
    }

    std::vector<core::NetworkInterface> interfaces;
    std::unordered_map<std::string, size_t> ifaceMap;

    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;

        std::string ifName = ifa->ifa_name;
        if (!ifaceMap.contains(ifName)) {
            core::NetworkInterface iface;
            iface.name = ifName;
            iface.isUp = (ifa->ifa_flags & IFF_UP) != 0;
            interfaces.push_back(std::move(iface));
            ifaceMap[ifName] = interfaces.size() - 1;
        }

        size_t idx = ifaceMap[ifName];
        int family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            char host[INET_ADDRSTRLEN] = {0};
            auto* sa = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
            inet_ntop(AF_INET, &(sa->sin_addr), host, sizeof(host));
            interfaces[idx].ipv4Addresses.emplace_back(host);
        } else if (family == AF_INET6) {
            char host[INET6_ADDRSTRLEN] = {0};
            auto* sa6 = reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr);
            inet_ntop(AF_INET6, &(sa6->sin6_addr), host, sizeof(host));
            interfaces[idx].ipv6Addresses.emplace_back(host);
        }
    }

    freeifaddrs(ifaddr);
    return core::Result<std::vector<core::NetworkInterface>>::ok(std::move(interfaces));
}

core::Result<std::vector<core::ListeningSocket>> LinuxNetworkManager::getListeningSockets() {
    std::vector<core::ListeningSocket> sockets;
    std::ifstream tcpFile("/proc/net/tcp");
    if (!tcpFile.is_open()) {
        return core::Result<std::vector<core::ListeningSocket>>::err(
            core::Error::make(core::ErrorCode::IoError, "Failed to open /proc/net/tcp.")
        );
    }

    std::string line;
    std::getline(tcpFile, line); // Skip header

    while (std::getline(tcpFile, line)) {
        std::istringstream iss(line);
        std::string sl, localAddressHex, remAddressHex, stHex;
        if (iss >> sl >> localAddressHex >> remAddressHex >> stHex) {
            // TCP_LISTEN is state 0x0A (10)
            if (stHex == "0A") {
                auto colonPos = localAddressHex.find(':');
                if (colonPos != std::string::npos) {
                    std::string ipHex = localAddressHex.substr(0, colonPos);
                    std::string portHex = localAddressHex.substr(colonPos + 1);

                    uint32_t ipVal = static_cast<uint32_t>(std::stoul(ipHex, nullptr, 16));
                    uint16_t portVal = static_cast<uint16_t>(std::stoul(portHex, nullptr, 16));

                    struct in_addr inAddr;
                    inAddr.s_addr = ipVal;
                    char ipStr[INET_ADDRSTRLEN] = {0};
                    inet_ntop(AF_INET, &inAddr, ipStr, sizeof(ipStr));

                    core::ListeningSocket sock;
                    sock.protocol = "tcp";
                    sock.localAddress = ipStr;
                    sock.localPort = portVal;
                    sockets.push_back(std::move(sock));
                }
            }
        }
    }

    return core::Result<std::vector<core::ListeningSocket>>::ok(std::move(sockets));
}

core::Result<bool> LinuxNetworkManager::isPortInUse(uint16_t port, const std::string& protocol) {
    (void)protocol;
    auto socketsRes = getListeningSockets();
    if (!socketsRes.isOk()) {
        return core::Result<bool>::err(socketsRes.error());
    }

    for (const auto& sock : socketsRes.value()) {
        if (sock.localPort == port) {
            return core::Result<bool>::ok(true);
        }
    }

    return core::Result<bool>::ok(false);
}

} // namespace space2x::platform::linux_os

#else

namespace space2x::platform::linux_os {

core::Result<std::vector<core::NetworkInterface>> LinuxNetworkManager::getInterfaces() {
    return core::Result<std::vector<core::NetworkInterface>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxNetworkManager is only supported on Linux.")
    );
}

core::Result<std::vector<core::ListeningSocket>> LinuxNetworkManager::getListeningSockets() {
    return core::Result<std::vector<core::ListeningSocket>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxNetworkManager is only supported on Linux.")
    );
}

core::Result<bool> LinuxNetworkManager::isPortInUse(uint16_t, const std::string&) {
    return core::Result<bool>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxNetworkManager is only supported on Linux.")
    );
}

} // namespace space2x::platform::linux_os

#endif
