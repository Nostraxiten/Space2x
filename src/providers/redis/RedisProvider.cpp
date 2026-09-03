#include "RedisProvider.h"
#include <space2x/core/IServiceManager.h>
#include <space2x/core/Engine.h>

namespace space2x::providers {

RedisProvider::RedisProvider() {
    m_manifest.id = "redis";
    m_manifest.displayName = "Redis In-Memory Data Store";
    m_manifest.version = "7.2.4";
    m_manifest.category = "cache";
    m_manifest.description = "In-memory data structure store used as a database, cache, message broker, and streaming engine";
    m_manifest.supportedPlatforms = {"windows", "linux"};
    m_manifest.supportedArchitectures = {"x86_64", "arm64"};
    m_manifest.defaultPort = 6379;
    m_manifest.serviceNames = {
        {"windows", "Redis"},
        {"linux", "redis-server.service"}
    };
    m_manifest.configPaths = {
        {"windows", {"C:\\Program Files\\Redis\\redis.conf"}},
        {"linux", {"/etc/redis/redis.conf", "/etc/redis.conf"}}
    };
    m_manifest.logPaths = {
        {"windows", {"C:\\Program Files\\Redis\\redis.log"}},
        {"linux", {"/var/log/redis/redis-server.log", "/var/log/redis/redis.log"}}
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
    const auto serviceName = m_manifest.serviceNameForCurrentPlatform();
    return engine.serviceManager().getState(serviceName);
}

core::Result<void> RedisProvider::performHealthCheck(const core::Engine& engine) const {
    auto stateRes = detectState(engine);
    if (!stateRes.isOk()) {
        return core::Result<void>::err(stateRes.error());
    }
    if (stateRes.value() != core::ServiceState::Running) {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::ServiceNotRunning,
            "Redis is not in a running state."
        ));
    }
    return core::Result<void>::ok();
}

} // namespace space2x::providers
