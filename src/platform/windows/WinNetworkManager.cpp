#include "WinNetworkManager.h"

#if defined(_WIN32)
// Target Windows 10 (0x0A00) — required for Vista+ networking APIs.
// Must be defined before any Windows header. Winsock2 must precede windows.h.
#ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0A00
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace space2x::platform::windows {

namespace {

std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) return {};
    std::string result(static_cast<size_t>(sizeNeeded), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), sizeNeeded, nullptr, nullptr);
    return result;
}

std::string formatMacAddress(const BYTE* mac, DWORD length) {
    if (!mac || length == 0) return "";
    std::ostringstream oss;
    for (DWORD i = 0; i < length; ++i) {
        if (i > 0) oss << ":";
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    }
    return oss.str();
}

} // anonymous namespace

core::Result<std::vector<core::NetworkInterface>> WinNetworkManager::getInterfaces() {
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;
    ULONG bufferSize = 15000;
    std::vector<BYTE> buffer(bufferSize);

    ULONG ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &bufferSize);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(bufferSize);
        ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &bufferSize);
    }

    if (ret != NO_ERROR) {
        return core::Result<std::vector<core::NetworkInterface>>::err(
            core::Error::make(core::ErrorCode::NetworkUnavailable, "Failed to query network adapter addresses.")
        );
    }

    std::vector<core::NetworkInterface> interfaces;
    auto* adapter = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    while (adapter) {
        core::NetworkInterface iface;
        iface.name = adapter->AdapterName ? adapter->AdapterName : "";
        if (adapter->Description) {
            iface.description = wideToUtf8(adapter->Description);
        }
        iface.isUp = (adapter->OperStatus == IfOperStatusUp);
        iface.macAddress = formatMacAddress(adapter->PhysicalAddress, adapter->PhysicalAddressLength);

        // IPv4 / IPv6 addresses
        auto* unicast = adapter->FirstUnicastAddress;
        while (unicast) {
            char ipStr[INET6_ADDRSTRLEN] = {0};
            auto* sockaddr = unicast->Address.lpSockaddr;
            if (sockaddr->sa_family == AF_INET) {
                auto* sin = reinterpret_cast<struct sockaddr_in*>(sockaddr);
                inet_ntop(AF_INET, &(sin->sin_addr), ipStr, sizeof(ipStr));
                iface.ipv4Addresses.emplace_back(ipStr);
            } else if (sockaddr->sa_family == AF_INET6) {
                auto* sin6 = reinterpret_cast<struct sockaddr_in6*>(sockaddr);
                inet_ntop(AF_INET6, &(sin6->sin6_addr), ipStr, sizeof(ipStr));
                iface.ipv6Addresses.emplace_back(ipStr);
            }
            unicast = unicast->Next;
        }

        // Gateway
        if (adapter->FirstGatewayAddress) {
            char gwStr[INET6_ADDRSTRLEN] = {0};
            auto* sockaddr = adapter->FirstGatewayAddress->Address.lpSockaddr;
            if (sockaddr->sa_family == AF_INET) {
                auto* sin = reinterpret_cast<struct sockaddr_in*>(sockaddr);
                inet_ntop(AF_INET, &(sin->sin_addr), gwStr, sizeof(gwStr));
                iface.defaultGateway = gwStr;
            }
        }

        interfaces.push_back(std::move(iface));
        adapter = adapter->Next;
    }

    return core::Result<std::vector<core::NetworkInterface>>::ok(std::move(interfaces));
}

core::Result<std::vector<core::ListeningSocket>> WinNetworkManager::getListeningSockets() {
    DWORD bufferSize = 0;
    GetExtendedTcpTable(nullptr, &bufferSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);

    std::vector<BYTE> buffer(bufferSize);
    DWORD ret = GetExtendedTcpTable(buffer.data(), &bufferSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    if (ret != NO_ERROR) {
        return core::Result<std::vector<core::ListeningSocket>>::err(
            core::Error::make(core::ErrorCode::NetworkUnavailable, "Failed to retrieve TCP socket table.")
        );
    }

    std::vector<core::ListeningSocket> sockets;
    auto* table = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buffer.data());

    for (DWORD i = 0; i < table->dwNumEntries; ++i) {
        const auto& row = table->table[i];
        if (row.dwState == MIB_TCP_STATE_LISTEN) {
            core::ListeningSocket sock;
            sock.protocol = "tcp";
            sock.localPort = ntohs(static_cast<u_short>(row.dwLocalPort));
            sock.pid = row.dwOwningPid;

            char ipStr[INET_ADDRSTRLEN] = {0};
            struct in_addr addr;
            addr.S_un.S_addr = row.dwLocalAddr;
            inet_ntop(AF_INET, &addr, ipStr, sizeof(ipStr));
            sock.localAddress = ipStr;

            sockets.push_back(std::move(sock));
        }
    }

    return core::Result<std::vector<core::ListeningSocket>>::ok(std::move(sockets));
}

core::Result<bool> WinNetworkManager::isPortInUse(uint16_t port, const std::string& protocol) {
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

} // namespace space2x::platform::windows

#else

namespace space2x::platform::windows {

core::Result<std::vector<core::NetworkInterface>> WinNetworkManager::getInterfaces() {
    return core::Result<std::vector<core::NetworkInterface>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinNetworkManager is only supported on Windows.")
    );
}

core::Result<std::vector<core::ListeningSocket>> WinNetworkManager::getListeningSockets() {
    return core::Result<std::vector<core::ListeningSocket>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinNetworkManager is only supported on Windows.")
    );
}

core::Result<bool> WinNetworkManager::isPortInUse(uint16_t, const std::string&) {
    return core::Result<bool>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinNetworkManager is only supported on Windows.")
    );
}

} // namespace space2x::platform::windows

#endif
