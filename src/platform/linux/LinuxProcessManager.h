#pragma once

#include <space2x/core/IProcessManager.h>

namespace space2x::platform::linux_os {

class LinuxProcessManager : public core::IProcessManager {
public:
    LinuxProcessManager() = default;
    ~LinuxProcessManager() override = default;

    [[nodiscard]] core::Result<std::vector<core::ProcessInfo>> listProcesses() override;
    [[nodiscard]] core::Result<core::ProcessInfo>              getProcessInfo(uint32_t pid) override;
    [[nodiscard]] core::Result<void>                         killProcess(uint32_t pid) override;
};

} // namespace space2x::platform::linux_os
