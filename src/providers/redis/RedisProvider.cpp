#include "RedisProvider.h"
#include <space2x/core/Engine.h>
#include <space2x/core/IServiceManager.h>

namespace space2x::providers {

RedisProvider::RedisProvider() {
    m_manifest.id                    = "redis";
    m_manifest.displayName           = "Redis In-Memory Data Store";
    m_manifest.version               = "7.2.4";
    m_manifest.category              = "cache";
    m_manifest.description           = "In-memory data structure store used as a database, "
                                       "cache, message broker, and streaming engine.";
    m_manifest.supportedPlatforms    = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort           = 6379;
    m_manifest.serviceNames          = {
        {"windows", "Redis"},
        {"linux",   "redis-server.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\Program Files\\Redis\\redis.conf"}},
        {"linux",   {"/etc/redis/redis.conf", "/etc/redis.conf"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\Program Files\\Redis\\redis.log"}},
        {"linux",   {"/var/log/redis/redis-server.log", "/var/log/redis/redis.log"}}
    };
    m_manifest.healthCheck = {
        "tcp",
        6379,
        "",
        1500
    };
}

RedisProvider::RedisProvider(provider::ProviderManifest manifest)
    : m_manifest(std::move(manifest)) {}

const provider::ProviderManifest& RedisProvider::manifest() const noexcept {
    return m_manifest;
}

core::Result<core::ServiceState> RedisProvider::detectState(const core::Engine& engine) const {
    const std::string serviceName = m_manifest.serviceNameForCurrentPlatform();
    auto stateRes = engine.serviceManager().getState(serviceName);
    if (!stateRes.isOk()) {
        return core::Result<core::ServiceState>::err(core::Error::make(
            core::ErrorCode::ServiceNotFound,
            "Failed to query state for Redis service '" + serviceName + "'.",
            stateRes.error().detail,
            stateRes.error().suggestion,
            std::make_shared<core::Error>(stateRes.error())
        ));
    }
    return stateRes;
}

core::Result<void> RedisProvider::performHealthCheck(const core::Engine& engine) const {
    auto stateRes = detectState(engine);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }

    const auto state = stateRes.value();
    switch (state) {
        case core::ServiceState::Running:
            return core::Result<void>::ok();

        case core::ServiceState::Starting:
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotRunning,
                "Redis is still starting.",
                "The service is in the 'Starting' state. Wait a moment and retry.",
                "Check 'systemctl status redis-server' or Windows Services for more detail."
            ));

        case core::ServiceState::Stopped:
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotRunning,
                "Redis is stopped.",
                "The service is not running.",
                "Start Redis with: space2x services start redis"
            ));

        case core::ServiceState::Degraded:
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotRunning,
                "Redis is in a degraded (failed) state.",
                "The service has reported a failure.",
                "Inspect logs with: journalctl -u redis-server --no-pager -n 50"
            ));

        case core::ServiceState::NotInstalled:
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotFound,
                "Redis does not appear to be installed.",
                "No Redis service unit was found on this system.",
                "Install Redis via your package manager and re-register the service."
            ));

        default:
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotRunning,
                "Redis is not in a running state.",
                "Current service state: " + std::string(core::serviceStateToString(state))
            ));
    }
}

} // namespace space2x::providers
