#include "SystemCommand.h"
#include <space2x/core/Engine.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

namespace space2x::cli {

namespace {

std::string formatBytes(uint64_t bytes) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    if (bytes >= GB) {
        oss << (static_cast<double>(bytes) / GB) << " GB";
    } else if (bytes >= MB) {
        oss << (static_cast<double>(bytes) / MB) << " MB";
    } else if (bytes >= KB) {
        oss << (static_cast<double>(bytes) / KB) << " KB";
    } else {
        oss << bytes << " B";
    }
    return oss.str();
}

} // anonymous namespace

int SystemCommand::execute(core::Engine& engine,
                           const std::vector<std::string>& args,
                           bool jsonMode) {
    if (args.empty() || args[0] == "info") {
        auto metricsRes = engine.systemMonitor().getMetrics();
        if (!metricsRes.isOk()) {
            std::cerr << metricsRes.error().toString() << "\n";
            return 1;
        }

        const auto& m = metricsRes.value();
        if (jsonMode) {
            nlohmann::json j = {
                {"cpuUsagePercent", m.cpuUsagePercent},
                {"totalMemoryBytes", m.totalPhysicalMemoryBytes},
                {"usedMemoryBytes", m.usedPhysicalMemoryBytes},
                {"totalSwapBytes", m.totalSwapBytes},
                {"usedSwapBytes", m.usedSwapBytes},
                {"uptimeSeconds", m.uptimeSeconds}
            };
            std::cout << j.dump(2) << "\n";
            return 0;
        }

        std::cout << "Space2X System Telemetry:\n";
        std::cout << std::string(40, '-') << "\n";
        std::cout << "CPU Usage:      " << std::fixed << std::setprecision(1) << m.cpuUsagePercent << " %\n";
        std::cout << "Memory Usage:   " << formatBytes(m.usedPhysicalMemoryBytes) << " / " << formatBytes(m.totalPhysicalMemoryBytes) << "\n";
        std::cout << "Swap / Paging:  " << formatBytes(m.usedSwapBytes) << " / " << formatBytes(m.totalSwapBytes) << "\n";
        std::cout << "System Uptime:  " << (m.uptimeSeconds / 3600) << "h " << ((m.uptimeSeconds % 3600) / 60) << "m " << (m.uptimeSeconds % 60) << "s\n";
        return 0;
    }

    if (args[0] == "processes") {
        auto procRes = engine.processManager().listProcesses();
        if (!procRes.isOk()) {
            std::cerr << procRes.error().toString() << "\n";
            return 1;
        }

        if (jsonMode) {
            nlohmann::json jArr = nlohmann::json::array();
            for (const auto& p : procRes.value()) {
                jArr.push_back({
                    {"pid", p.pid},
                    {"ppid", p.parentPid},
                    {"name", p.name},
                    {"memoryBytes", p.memoryBytes},
                    {"executablePath", p.executablePath}
                });
            }
            std::cout << jArr.dump(2) << "\n";
            return 0;
        }

        std::cout << std::left
                  << std::setw(10) << "PID"
                  << std::setw(10) << "PPID"
                  << std::setw(30) << "PROCESS NAME"
                  << std::setw(15) << "MEMORY"
                  << "\n";
        std::cout << std::string(65, '-') << "\n";

        for (const auto& p : procRes.value()) {
            std::cout << std::left
                      << std::setw(10) << p.pid
                      << std::setw(10) << p.parentPid
                      << std::setw(30) << p.name.substr(0, 28)
                      << std::setw(15) << formatBytes(p.memoryBytes)
                      << "\n";
        }
        return 0;
    }

    std::cerr << "Unknown system sub-command: " << args[0] << "\n";
    return 1;
}

} // namespace space2x::cli
