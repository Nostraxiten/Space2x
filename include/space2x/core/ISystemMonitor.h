#pragma once

#include "Error.h"
#include "Result.h"

#include <cstdint>

namespace space2x::core {

struct SystemMetrics {
    double   cpuUsagePercent{0.0};
    uint64_t totalPhysicalMemoryBytes{0};
    uint64_t usedPhysicalMemoryBytes{0};
    uint64_t totalSwapBytes{0};
    uint64_t usedSwapBytes{0};
    uint64_t uptimeSeconds{0};
};

class ISystemMonitor {
public:
    virtual ~ISystemMonitor() = default;

    [[nodiscard]] virtual Result<SystemMetrics> getMetrics() = 0;
};

} // namespace space2x::core
