#include "OpenSSHProvider.h"
#include <space2x/core/IServiceManager.h>

namespace space2x::providers {

OpenSSHProvider::OpenSSHProvider() {
    m_manifest.id = "openssh";
    m_manifest.displayName = "OpenSSH Server";
    m_manifest.version = "9.6.0";
    m_manifest.category = "remote-access";
    m_manifest.description = "Secure Shell daemon for encrypted remote login and command execution";
    m_manifest.supportedPlatforms = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort = 22;
    m_manifest.serviceNames = {
        {"windows", "sshd"},
        {"linux", "sshd.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\ProgramData\\ssh\\sshd_config"}},
        {"linux", {"/etc/ssh/sshd_config"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\ProgramData\\ssh\\logs\\sshd.log"}},
        {"linux", {"/var/log/auth.log", "/var/log/secure"}}
    };
    m_manifest.healthCheck = {
        "tcp",
        22,
        "",
        1500
    };
}

OpenSSHProvider::OpenSSHProvider(provider::ProviderManifest manifest)
    : m_manifest(std::move(manifest)) {}

const provider::ProviderManifest& OpenSSHProvider::manifest() const noexcept {
    return m_manifest;
}

core::Result<core::ServiceState> OpenSSHProvider::detectState(core::IServiceManager& serviceManager) const {
    const auto serviceName = m_manifest.serviceNameForCurrentPlatform();
    return serviceManager.getState(serviceName);
}

core::Result<void> OpenSSHProvider::performHealthCheck(core::IServiceManager& serviceManager) const {
    auto stateRes = detectState(serviceManager);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }
    if (stateRes.value() != core::ServiceState::Running) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceNotRunning,
            "OpenSSH is not in a running state."
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::providers
