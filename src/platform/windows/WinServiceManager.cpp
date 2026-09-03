#include "WinServiceManager.h"

#if defined(_WIN32)
// Target Windows 10 (0x0A00) — required for QueryServiceStatusProcess
// and other Vista+ SCM APIs. Must be defined before any Windows header.
#ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0A00
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winsvc.h>
#include <thread>
#include <chrono>

namespace space2x::platform::windows {

namespace {

struct ScHandleCloser {
    void operator()(SC_HANDLE h) const noexcept {
        if (h) {
            CloseServiceHandle(h);
        }
    }
};

using UniqueScHandle = std::unique_ptr<std::remove_pointer_t<SC_HANDLE>, ScHandleCloser>;

std::wstring utf8ToWide(const std::string& str) {
    if (str.empty()) return {};
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    if (sizeNeeded <= 0) return {};
    std::wstring result(static_cast<size_t>(sizeNeeded), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded);
    return result;
}

std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) return {};
    std::string result(static_cast<size_t>(sizeNeeded), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), sizeNeeded, nullptr, nullptr);
    return result;
}

core::ServiceState mapWin32ServiceState(DWORD currentStatus) {
    switch (currentStatus) {
        case SERVICE_START_PENDING: return core::ServiceState::Starting;
        case SERVICE_RUNNING:       return core::ServiceState::Running;
        case SERVICE_STOP_PENDING:  return core::ServiceState::Stopping;
        case SERVICE_STOPPED:       return core::ServiceState::Stopped;
        case SERVICE_PAUSE_PENDING:
        case SERVICE_PAUSED:        return core::ServiceState::Degraded;
        default:                    return core::ServiceState::Unknown;
    }
}

core::Error makeWin32Error(DWORD errCode, const std::string& action, const std::string& serviceId) {
    std::string detail = "Win32 error code: " + std::to_string(errCode);
    std::string suggestion = "Ensure the service is installed and that Space2X has appropriate privileges.";
    core::ErrorCode code = core::ErrorCode::Unknown;

    if (errCode == ERROR_ACCESS_DENIED) {
        code = core::ErrorCode::PermissionDenied;
        suggestion = "Run Space2X with elevated Administrator privileges to manage this service.";
    } else if (errCode == ERROR_SERVICE_DOES_NOT_EXIST) {
        code = core::ErrorCode::ServiceNotFound;
        suggestion = "Verify the service name and ensure the application is installed on the host.";
    } else if (errCode == ERROR_SERVICE_ALREADY_RUNNING) {
        code = core::ErrorCode::ServiceAlreadyRunning;
        suggestion = "Service is already in a running state.";
    } else if (errCode == ERROR_SERVICE_NOT_ACTIVE) {
        code = core::ErrorCode::ServiceNotRunning;
        suggestion = "Service is already stopped.";
    }

    return core::Error::make(
        code,
        "Failed to " + action + " service '" + serviceId + "'.",
        detail,
        suggestion
    );
}

} // anonymous namespace

core::Result<std::vector<core::ServiceInfo>> WinServiceManager::listServices() {
    UniqueScHandle scm(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT));
    if (!scm) {
        return core::Result<std::vector<core::ServiceInfo>>::err(
            makeWin32Error(GetLastError(), "open Service Control Manager for enumeration", "")
        );
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;

    EnumServicesStatusExW(
        scm.get(),
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        nullptr,
        0,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr
    );

    if (GetLastError() != ERROR_MORE_DATA && GetLastError() != ERROR_SUCCESS) {
        return core::Result<std::vector<core::ServiceInfo>>::err(
            makeWin32Error(GetLastError(), "enumerate services", "")
        );
    }

    std::vector<BYTE> buffer(bytesNeeded);
    if (!EnumServicesStatusExW(
            scm.get(),
            SC_ENUM_PROCESS_INFO,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            buffer.data(),
            bytesNeeded,
            &bytesNeeded,
            &servicesReturned,
            &resumeHandle,
            nullptr)) {
        return core::Result<std::vector<core::ServiceInfo>>::err(
            makeWin32Error(GetLastError(), "enumerate services", "")
        );
    }

    auto* services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESSW>(buffer.data());
    std::vector<core::ServiceInfo> result;
    result.reserve(servicesReturned);

    for (DWORD i = 0; i < servicesReturned; ++i) {
        core::ServiceInfo info;
        info.id = wideToUtf8(services[i].lpServiceName);
        info.displayName = wideToUtf8(services[i].lpDisplayName);
        info.state = mapWin32ServiceState(services[i].ServiceStatusProcess.dwCurrentState);
        info.pid = services[i].ServiceStatusProcess.dwProcessId;
        result.push_back(std::move(info));
    }

    return core::Result<std::vector<core::ServiceInfo>>::ok(std::move(result));
}

core::Result<core::ServiceState> WinServiceManager::getState(const std::string& serviceId) {
    UniqueScHandle scm(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT));
    if (!scm) {
        return core::Result<core::ServiceState>::err(
            makeWin32Error(GetLastError(), "connect to SCM", serviceId)
        );
    }

    auto wideId = utf8ToWide(serviceId);
    UniqueScHandle service(OpenServiceW(scm.get(), wideId.c_str(), SERVICE_QUERY_STATUS));
    if (!service) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_DOES_NOT_EXIST) {
            return core::Result<core::ServiceState>::ok(core::ServiceState::NotInstalled);
        }
        return core::Result<core::ServiceState>::err(
            makeWin32Error(err, "query status of", serviceId)
        );
    }

    SERVICE_STATUS_PROCESS ssp{};
    DWORD bytesNeeded = 0;
    if (!QueryServiceStatusProcess(
            service.get(),
            SC_STATUS_PROCESS_INFO,
            reinterpret_cast<LPBYTE>(&ssp),
            sizeof(ssp),
            &bytesNeeded)) {
        return core::Result<core::ServiceState>::err(
            makeWin32Error(GetLastError(), "query status process for", serviceId)
        );
    }

    return core::Result<core::ServiceState>::ok(mapWin32ServiceState(ssp.dwCurrentState));
}

core::Result<void> WinServiceManager::start(const std::string& serviceId) {
    UniqueScHandle scm(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT));
    if (!scm) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "connect to SCM", serviceId));
    }

    auto wideId = utf8ToWide(serviceId);
    UniqueScHandle service(OpenServiceW(scm.get(), wideId.c_str(), SERVICE_START | SERVICE_QUERY_STATUS));
    if (!service) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "open", serviceId));
    }

    if (!StartServiceW(service.get(), 0, nullptr)) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_ALREADY_RUNNING) {
            return core::Result<void>::ok();
        }
        return core::Result<void>::err(makeWin32Error(err, "start", serviceId));
    }

    return core::Result<void>::ok();
}

core::Result<void> WinServiceManager::stop(const std::string& serviceId) {
    UniqueScHandle scm(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT));
    if (!scm) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "connect to SCM", serviceId));
    }

    auto wideId = utf8ToWide(serviceId);
    UniqueScHandle service(OpenServiceW(scm.get(), wideId.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS));
    if (!service) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "open", serviceId));
    }

    SERVICE_STATUS status{};
    if (!ControlService(service.get(), SERVICE_CONTROL_STOP, &status)) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_NOT_ACTIVE) {
            return core::Result<void>::ok();
        }
        return core::Result<void>::err(makeWin32Error(err, "stop", serviceId));
    }

    return core::Result<void>::ok();
}

core::Result<void> WinServiceManager::restart(const std::string& serviceId) {
    auto stopRes = stop(serviceId);
    if (!stopRes.isOk() && stopRes.error().code != core::ErrorCode::ServiceNotRunning) {
        return stopRes;
    }

    // Wait briefly for stopped status
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return start(serviceId);
}

core::Result<void> WinServiceManager::setAutoStart(const std::string& serviceId, bool enabled) {
    UniqueScHandle scm(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT));
    if (!scm) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "connect to SCM", serviceId));
    }

    auto wideId = utf8ToWide(serviceId);
    UniqueScHandle service(OpenServiceW(scm.get(), wideId.c_str(), SERVICE_CHANGE_CONFIG));
    if (!service) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "open for configuration", serviceId));
    }

    DWORD startType = enabled ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
    if (!ChangeServiceConfigW(
            service.get(),
            SERVICE_NO_CHANGE,
            startType,
            SERVICE_NO_CHANGE,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr)) {
        return core::Result<void>::err(makeWin32Error(GetLastError(), "change auto-start config of", serviceId));
    }

    return core::Result<void>::ok();
}

} // namespace space2x::platform::windows

#else

namespace space2x::platform::windows {

core::Result<std::vector<core::ServiceInfo>> WinServiceManager::listServices() {
    return core::Result<std::vector<core::ServiceInfo>>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

core::Result<core::ServiceState> WinServiceManager::getState(const std::string&) {
    return core::Result<core::ServiceState>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

core::Result<void> WinServiceManager::start(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

core::Result<void> WinServiceManager::stop(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

core::Result<void> WinServiceManager::restart(const std::string&) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

core::Result<void> WinServiceManager::setAutoStart(const std::string&, bool) {
    return core::Result<void>::err(
        core::Error::make(core::ErrorCode::PlatformNotSupported, "Windows Service Manager is only supported on Windows.")
    );
}

} // namespace space2x::platform::windows

#endif
