#include "LinuxProcessManager.h"

#if defined(__linux__)
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cctype>
#include <fstream>
#include <sstream>

namespace space2x::platform::linux_os {

namespace {

bool isAllDigits(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

} // anonymous namespace

core::Result<std::vector<core::ProcessInfo>> LinuxProcessManager::listProcesses() {
    DIR* dir = opendir("/proc");
    if (!dir) {
        return core::Result<std::vector<core::ProcessInfo>>::err(
            core::Error::make(core::ErrorCode::IoError, "Failed to open /proc directory.")
        );
    }

    std::vector<core::ProcessInfo> processes;
    struct dirent* entry = nullptr;

    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && isAllDigits(entry->d_name)) {
            uint32_t pid = static_cast<uint32_t>(std::stoul(entry->d_name));
            auto pInfo = getProcessInfo(pid);
            if (pInfo.isOk()) {
                processes.push_back(pInfo.value());
            }
        }
    }

    closedir(dir);
    return core::Result<std::vector<core::ProcessInfo>>::ok(std::move(processes));
}

core::Result<core::ProcessInfo> LinuxProcessManager::getProcessInfo(uint32_t pid) {
    std::string pidStr = std::to_string(pid);
    std::ifstream statFile("/proc/" + pidStr + "/stat");
    if (!statFile.is_open()) {
        return core::Result<core::ProcessInfo>::err(
            core::Error::make(core::ErrorCode::ProcessNotFound, "Process with PID " + pidStr + " not found.")
        );
    }

    core::ProcessInfo info;
    info.pid = pid;

    std::string line;
    if (std::getline(statFile, line)) {
        auto openParen = line.find('(');
        auto closeParen = line.rfind(')');
        if (openParen != std::string::npos && closeParen != std::string::npos) {
            info.name = line.substr(openParen + 1, closeParen - openParen - 1);

            std::string rest = line.substr(closeParen + 2);
            std::istringstream iss(rest);
            char state;
            uint32_t ppid;
            iss >> state >> ppid;
            info.parentPid = ppid;
        }
    }

    // Read cmdline for executable path
    std::ifstream cmdlineFile("/proc/" + pidStr + "/cmdline");
    if (cmdlineFile.is_open()) {
        std::string cmd;
        if (std::getline(cmdlineFile, cmd, '\0')) {
            info.executablePath = cmd;
        }
    }

    // Read memory usage from statm (resident set size in pages)
    std::ifstream statmFile("/proc/" + pidStr + "/statm");
    if (statmFile.is_open()) {
        uint64_t sizePages, residentPages;
        if (statmFile >> sizePages >> residentPages) {
            long pageSize = sysconf(_SC_PAGESIZE);
            info.memoryBytes = residentPages * static_cast<uint64_t>(pageSize > 0 ? pageSize : 4096);
        }
    }

    return core::Result<core::ProcessInfo>::ok(std::move(info));
}

core::Result<void> LinuxProcessManager::killProcess(uint32_t pid) {
    int ret = kill(static_cast<pid_t>(pid), SIGTERM);
    if (ret != 0) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::PermissionDenied,
            "Failed to send SIGTERM to process " + std::to_string(pid)
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::platform::linux_os

#else

namespace space2x::platform::linux_os {

core::Result<std::vector<core::ProcessInfo>> LinuxProcessManager::listProcesses() {
    return core::Result<std::vector<core::ProcessInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxProcessManager is only supported on Linux.")
    );
}

core::Result<core::ProcessInfo> LinuxProcessManager::getProcessInfo(uint32_t) {
    return core::Result<core::ProcessInfo>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxProcessManager is only supported on Linux.")
    );
}

core::Result<void> LinuxProcessManager::killProcess(uint32_t) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "LinuxProcessManager is only supported on Linux.")
    );
}

} // namespace space2x::platform::linux_os

#endif
