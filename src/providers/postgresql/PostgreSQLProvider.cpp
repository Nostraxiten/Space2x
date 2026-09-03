#include "PostgreSQLProvider.h"
#include <space2x/core/IServiceManager.h>

namespace space2x::providers {

PostgreSQLProvider::PostgreSQLProvider() {
    m_manifest.id = "postgresql";
    m_manifest.displayName = "PostgreSQL Database Server";
    m_manifest.version = "16.2";
    m_manifest.category = "database";
    m_manifest.description = "Powerful, open-source object-relational database system";
    m_manifest.supportedPlatforms = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort = 5432;
    m_manifest.serviceNames = {
        {"windows", "postgresql-x64-16"},
        {"linux", "postgresql.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\Program Files\\PostgreSQL\\16\\data\\postgresql.conf"}},
        {"linux", {"/etc/postgresql/16/main/postgresql.conf", "/var/lib/pgsql/data/postgresql.conf"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\Program Files\\PostgreSQL\\16\\data\\log\\"}},
        {"linux", {"/var/log/postgresql/"}}
    };
    m_manifest.healthCheck = {
        "tcp",
        5432,
        "",
        2000
    };
}

PostgreSQLProvider::PostgreSQLProvider(provider::ProviderManifest manifest)
    : m_manifest(std::move(manifest)) {}

const provider::ProviderManifest& PostgreSQLProvider::manifest() const noexcept {
    return m_manifest;
}

core::Result<core::ServiceState> PostgreSQLProvider::detectState(core::IServiceManager& serviceManager) const {
    const auto serviceName = m_manifest.serviceNameForCurrentPlatform();
    return serviceManager.getState(serviceName);
}

core::Result<void> PostgreSQLProvider::performHealthCheck(core::IServiceManager& serviceManager) const {
    auto stateRes = detectState(serviceManager);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }
    if (stateRes.value() != core::ServiceState::Running) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceNotRunning,
            "PostgreSQL is not in a running state."
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::providers
