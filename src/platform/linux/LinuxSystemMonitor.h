#pragma once

#include <space2x/core/ISystemMonitor.h>
#include <cstdint>

namespace space2x::platform::linux_os {

class LinuxSystemMonitor : public core::ISystemMonitor {
public:
    LinuxSystemMonitor();
    ~LinuxSystemMonitor() override = default;

    [[nodiscard]] core::Result<core::SystemMetrics> getMetrics() override;

private:
    uint64_t m_lastIdleTime{0};
    uint64_t m_lastTotalTime{0};
    bool     m_hasPreviousTimes{false};
};

} // namespace space2x::platform::linux_os
