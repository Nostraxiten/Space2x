#include "NginxProvider.h"
#include <space2x/core/IServiceManager.h>

namespace space2x::providers {

NginxProvider::NginxProvider() {
    m_manifest.id = "nginx";
    m_manifest.displayName = "Nginx HTTP & Reverse Proxy";
    m_manifest.version = "1.24.0";
    m_manifest.category = "web-server";
    m_manifest.description = "High-performance HTTP server, reverse proxy, and IMAP/POP3 proxy server";
    m_manifest.supportedPlatforms = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort = 80;
    m_manifest.serviceNames = {
        {"windows", "nginx"},
        {"linux", "nginx.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\nginx\\conf\\nginx.conf"}},
        {"linux", {"/etc/nginx/nginx.conf"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\nginx\\logs\\error.log", "C:\\nginx\\logs\\access.log"}},
        {"linux", {"/var/log/nginx/error.log", "/var/log/nginx/access.log"}}
    };
    m_manifest.healthCheck = {
        "tcp",
        80,
        "",
        1500
    };
}

NginxProvider::NginxProvider(provider::ProviderManifest manifest)
    : m_manifest(std::move(manifest)) {}

const provider::ProviderManifest& NginxProvider::manifest() const noexcept {
    return m_manifest;
}

core::Result<core::ServiceState> NginxProvider::detectState(core::IServiceManager& serviceManager) const {
    const auto serviceName = m_manifest.serviceNameForCurrentPlatform();
    return serviceManager.getState(serviceName);
}

core::Result<void> NginxProvider::performHealthCheck(core::IServiceManager& serviceManager) const {
    auto stateRes = detectState(serviceManager);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }
    if (stateRes.value() != core::ServiceState::Running) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceNotRunning,
            "Nginx is not in a running state."
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::providers
