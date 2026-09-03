#include "WinProcessManager.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

namespace space2x::platform::windows {

namespace {

std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) return {};
    std::string result(static_cast<size_t>(sizeNeeded), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), sizeNeeded, nullptr, nullptr);
    return result;
}

} // anonymous namespace

core::Result<std::vector<core::ProcessInfo>> WinProcessManager::listProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return core::Result<std::vector<core::ProcessInfo>>::err(
            core::Error::make(core::ErrorCode::Unknown, "Failed to create process snapshot.")
        );
    }

    std::vector<core::ProcessInfo> processes;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &entry)) {
        do {
            core::ProcessInfo info;
            info.pid = entry.th32ProcessID;
            info.parentPid = entry.th32ParentProcessID;
            info.name = wideToUtf8(entry.szExeFile);

            // Fetch memory info if possible
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
            if (hProcess) {
                PROCESS_MEMORY_COUNTERS pmc{};
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    info.memoryBytes = pmc.WorkingSetSize;
                }
                WCHAR path[MAX_PATH];
                DWORD pathLen = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, path, &pathLen)) {
                    info.executablePath = wideToUtf8(path);
                }
                CloseHandle(hProcess);
            }

            processes.push_back(std::move(info));
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return core::Result<std::vector<core::ProcessInfo>>::ok(std::move(processes));
}

core::Result<core::ProcessInfo> WinProcessManager::getProcessInfo(uint32_t pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) {
        return core::Result<core::ProcessInfo>::err(
            core::Error::make(core::ErrorCode::ProcessNotFound, "Process with PID " + std::to_string(pid) + " not found or inaccessible.")
        );
    }

    core::ProcessInfo info;
    info.pid = pid;

    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        info.memoryBytes = pmc.WorkingSetSize;
    }

    WCHAR path[MAX_PATH];
    DWORD pathLen = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, path, &pathLen)) {
        info.executablePath = wideToUtf8(path);
        std::wstring fullPath(path);
        auto pos = fullPath.find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            info.name = wideToUtf8(fullPath.substr(pos + 1));
        } else {
            info.name = info.executablePath;
        }
    }

    CloseHandle(hProcess);
    return core::Result<core::ProcessInfo>::ok(std::move(info));
}

core::Result<void> WinProcessManager::killProcess(uint32_t pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!hProcess) {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::PermissionDenied,
                "Permission denied when attempting to terminate process " + std::to_string(pid),
                "Win32 error: " + std::to_string(err),
                "Run Space2X as Administrator to terminate protected processes."
            ));
        }
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ProcessNotFound,
            "Cannot open process " + std::to_string(pid) + " for termination."
        ));
    }

    if (!TerminateProcess(hProcess, 1)) {
        DWORD err = GetLastError();
        CloseHandle(hProcess);
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::Unknown,
            "Failed to terminate process " + std::to_string(pid),
            "Win32 error: " + std::to_string(err)
        ));
    }

    CloseHandle(hProcess);
    return core::Result<void>::ok();
}

} // namespace space2x::platform::windows

#else

namespace space2x::platform::windows {

core::Result<std::vector<core::ProcessInfo>> WinProcessManager::listProcesses() {
    return core::Result<std::vector<core::ProcessInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinProcessManager is only supported on Windows.")
    );
}

core::Result<core::ProcessInfo> WinProcessManager::getProcessInfo(uint32_t) {
    return core::Result<core::ProcessInfo>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinProcessManager is only supported on Windows.")
    );
}

core::Result<void> WinProcessManager::killProcess(uint32_t) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "WinProcessManager is only supported on Windows.")
    );
}

} // namespace space2x::platform::windows

#endif
