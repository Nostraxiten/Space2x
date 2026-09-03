#include "MySQLProvider.h"
#include <space2x/core/IServiceManager.h>
#include <space2x/core/Engine.h>

namespace space2x::providers {

MySQLProvider::MySQLProvider() {
    m_manifest.id = "mysql";
    m_manifest.displayName = "MySQL Server";
    m_manifest.version = "8.0.36";
    m_manifest.category = "database";
    m_manifest.description = "Widely deployed open-source relational database management system";
    m_manifest.supportedPlatforms = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort = 3306;
    m_manifest.serviceNames = {
        {"windows", "MySQL80"},
        {"linux", "mysql.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\ProgramData\\MySQL\\MySQL Server 8.0\\my.ini"}},
        {"linux", {"/etc/mysql/my.cnf", "/etc/my.cnf"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Data\\*.err"}},
        {"linux", {"/var/log/mysql/error.log"}}
    };
    m_manifest.healthCheck = {
        "tcp",
        3306,
        "",
        2000
    };
}

MySQLProvider::MySQLProvider(provider::ProviderManifest manifest)
    : m_manifest(std::move(manifest)) {}

const provider::ProviderManifest& MySQLProvider::manifest() const noexcept {
    return m_manifest;
}

core::Result<core::ServiceState> MySQLProvider::detectState(const core::Engine& engine) const {
    const auto serviceName = m_manifest.serviceNameForCurrentPlatform();
    return engine.serviceManager().getState(serviceName);
}

core::Result<void> MySQLProvider::performHealthCheck(const core::Engine& engine) const {
    auto stateRes = detectState(engine);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }
    if (stateRes.value() != core::ServiceState::Running) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceNotRunning,
            "MySQL is not in a running state."
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::providers
