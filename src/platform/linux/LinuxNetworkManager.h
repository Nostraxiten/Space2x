#pragma once

#include <space2x/core/INetworkManager.h>

namespace space2x::platform::linux_os {

class LinuxNetworkManager : public core::INetworkManager {
public:
    LinuxNetworkManager() = default;
    ~LinuxNetworkManager() override = default;

    [[nodiscard]] core::Result<std::vector<core::NetworkInterface>> getInterfaces() override;
    [[nodiscard]] core::Result<std::vector<core::ListeningSocket>>  getListeningSockets() override;
    [[nodiscard]] core::Result<bool>                             isPortInUse(uint16_t port, const std::string& protocol = "tcp") override;
};

} // namespace space2x::platform::linux_os
