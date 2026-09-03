#pragma once

#include <space2x/core/INetworkManager.h>

namespace space2x::platform::windows {

class WinNetworkManager : public core::INetworkManager {
public:
    WinNetworkManager() = default;
    ~WinNetworkManager() override = default;

    [[nodiscard]] core::Result<std::vector<core::NetworkInterface>> getInterfaces() override;
    [[nodiscard]] core::Result<std::vector<core::ListeningSocket>>  getListeningSockets() override;
    [[nodiscard]] core::Result<bool>                             isPortInUse(uint16_t port, const std::string& protocol) override;
};

} // namespace space2x::platform::windows
