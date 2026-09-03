#pragma once

#include <space2x/core/IServiceManager.h>

namespace space2x::platform::windows {

class WinServiceManager : public core::IServiceManager {
public:
    WinServiceManager() = default;
    ~WinServiceManager() override = default;

    [[nodiscard]] core::Result<std::vector<core::ServiceInfo>> listServices() override;
    [[nodiscard]] core::Result<core::ServiceState>             getState(const std::string& serviceId) override;
    [[nodiscard]] core::Result<void>                         start(const std::string& serviceId) override;
    [[nodiscard]] core::Result<void>                         stop(const std::string& serviceId) override;
    [[nodiscard]] core::Result<void>                         restart(const std::string& serviceId) override;
    [[nodiscard]] core::Result<void>                         setAutoStart(const std::string& serviceId, bool enabled) override;
};

} // namespace space2x::platform::windows
