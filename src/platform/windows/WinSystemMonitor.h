#pragma once

#include <space2x/core/ISystemMonitor.h>
#include <cstdint>

namespace space2x::platform::windows {

class WinSystemMonitor : public core::ISystemMonitor {
public:
    WinSystemMonitor();
    ~WinSystemMonitor() override = default;

    [[nodiscard]] core::Result<core::SystemMetrics> getMetrics() override;

private:
    uint64_t m_lastIdleTime{0};
    uint64_t m_lastKernelTime{0};
    uint64_t m_lastUserTime{0};
    bool     m_hasPreviousTimes{false};
};

} // namespace space2x::platform::windows
