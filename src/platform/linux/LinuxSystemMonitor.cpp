#include "LinuxSystemMonitor.h"

#if defined(__linux__)
#include <fstream>
#include <sstream>
#include <string>

namespace space2x::platform::linux_os {

LinuxSystemMonitor::LinuxSystemMonitor() {
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string cpu;
        uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
        if (statFile >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
            m_lastIdleTime = idle + iowait;
            m_lastTotalTime = user + nice + system + idle + iowait + irq + softirq + steal;
            m_hasPreviousTimes = true;
        }
    }
}

core::Result<core::SystemMetrics> LinuxSystemMonitor::getMetrics() {
    core::SystemMetrics metrics{};

    // 1. Read /proc/meminfo
    std::ifstream memFile("/proc/meminfo");
    if (memFile.is_open()) {
        std::string line;
        uint64_t memTotal = 0, memAvailable = 0, swapTotal = 0, swapFree = 0;
        while (std::getline(memFile, line)) {
            std::istringstream iss(line);
            std::string key;
            uint64_t valueKb;
            iss >> key >> valueKb;
            if (key == "MemTotal:") memTotal = valueKb * 1024;
            else if (key == "MemAvailable:") memAvailable = valueKb * 1024;
            else if (key == "SwapTotal:") swapTotal = valueKb * 1024;
            else if (key == "SwapFree:") swapFree = valueKb * 1024;
        }
        metrics.totalPhysicalMemoryBytes = memTotal;
        metrics.usedPhysicalMemoryBytes = memTotal > memAvailable ? memTotal - memAvailable : 0;
        metrics.totalSwapBytes = swapTotal;
        metrics.usedSwapBytes = swapTotal > swapFree ? swapTotal - swapFree : 0;
    }

    // 2. Read /proc/uptime
    std::ifstream uptimeFile("/proc/uptime");
    if (uptimeFile.is_open()) {
        double uptimeSec;
        if (uptimeFile >> uptimeSec) {
            metrics.uptimeSeconds = static_cast<uint64_t>(uptimeSec);
        }
    }

    // 3. Read /proc/stat for CPU %
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string cpu;
        uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
        if (statFile >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
            uint64_t currentIdle = idle + iowait;
            uint64_t currentTotal = user + nice + system + idle + iowait + irq + softirq + steal;

            if (m_hasPreviousTimes) {
                uint64_t totalDiff = currentTotal - m_lastTotalTime;
                uint64_t idleDiff = currentIdle - m_lastIdleTime;
                if (totalDiff > 0) {
                    metrics.cpuUsagePercent = (1.0 - (static_cast<double>(idleDiff) / static_cast<double>(totalDiff))) * 100.0;
                    if (metrics.cpuUsagePercent < 0.0) metrics.cpuUsagePercent = 0.0;
                    if (metrics.cpuUsagePercent > 100.0) metrics.cpuUsagePercent = 100.0;
                }
            }

            m_lastIdleTime = currentIdle;
            m_lastTotalTime = currentTotal;
            m_hasPreviousTimes = true;
        }
    }

    return core::Result<core::SystemMetrics>::ok(metrics);
}

} // namespace space2x::platform::linux_os

#else

namespace space2x::platform::linux_os {

LinuxSystemMonitor::LinuxSystemMonitor() = default;

core::Result<core::SystemMetrics> LinuxSystemMonitor::getMetrics() {
    return core::Result<core::SystemMetrics>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxSystemMonitor is only supported on Linux.")
    );
}

} // namespace space2x::platform::linux_os

#endif
