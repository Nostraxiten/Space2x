#include "WinSystemMonitor.h"

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0A00
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace space2x::platform::windows {

namespace {

uint64_t fileTimeToUint64(const FILETIME& ft) {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

} // anonymous namespace

WinSystemMonitor::WinSystemMonitor() {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        m_lastIdleTime = fileTimeToUint64(idleTime);
        m_lastKernelTime = fileTimeToUint64(kernelTime);
        m_lastUserTime = fileTimeToUint64(userTime);
        m_hasPreviousTimes = true;
    }
}

core::Result<core::SystemMetrics> WinSystemMonitor::getMetrics() {
    core::SystemMetrics metrics{};

    // Memory info
    MEMORYSTATUSEX memStatus{};
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memStatus)) {
        metrics.totalPhysicalMemoryBytes = memStatus.ullTotalPhys;
        metrics.usedPhysicalMemoryBytes = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
        metrics.totalSwapBytes = memStatus.ullTotalPageFile;
        metrics.usedSwapBytes = memStatus.ullTotalPageFile - memStatus.ullAvailPageFile;
    }

    // Uptime
    metrics.uptimeSeconds = GetTickCount64() / 1000;

    // CPU calculation via GetSystemTimes delta
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        uint64_t currentIdle = fileTimeToUint64(idleTime);
        uint64_t currentKernel = fileTimeToUint64(kernelTime);
        uint64_t currentUser = fileTimeToUint64(userTime);

        if (m_hasPreviousTimes) {
            uint64_t idleDiff = currentIdle - m_lastIdleTime;
            uint64_t kernelDiff = currentKernel - m_lastKernelTime;
            uint64_t userDiff = currentUser - m_lastUserTime;

            uint64_t totalSys = kernelDiff + userDiff;
            if (totalSys > 0) {
                metrics.cpuUsagePercent = (1.0 - (static_cast<double>(idleDiff) / static_cast<double>(totalSys))) * 100.0;
                if (metrics.cpuUsagePercent < 0.0) metrics.cpuUsagePercent = 0.0;
                if (metrics.cpuUsagePercent > 100.0) metrics.cpuUsagePercent = 100.0;
            }
        }

        m_lastIdleTime = currentIdle;
        m_lastKernelTime = currentKernel;
        m_lastUserTime = currentUser;
        m_hasPreviousTimes = true;
    }

    return core::Result<core::SystemMetrics>::ok(metrics);
}

} // namespace space2x::platform::windows

#else

namespace space2x::platform::windows {

WinSystemMonitor::WinSystemMonitor() = default;

core::Result<core::SystemMetrics> WinSystemMonitor::getMetrics() {
    return core::Result<core::SystemMetrics>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinSystemMonitor is only supported on Windows.")
    );
}

} // namespace space2x::platform::windows

#endif
