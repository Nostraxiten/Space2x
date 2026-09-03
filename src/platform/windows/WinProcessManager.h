#pragma once

#include <space2x/core/IProcessManager.h>

namespace space2x::platform::windows {

class WinProcessManager : public core::IProcessManager {
public:
    WinProcessManager() = default;
    ~WinProcessManager() override = default;

    [[nodiscard]] core::Result<std::vector<core::ProcessInfo>> listProcesses() override;
    [[nodiscard]] core::Result<core::ProcessInfo>              getProcessInfo(uint32_t pid) override;
    [[nodiscard]] core::Result<void>                         killProcess(uint32_t pid) override;
};

} // namespace space2x::platform::windows
