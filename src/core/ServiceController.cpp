#include "ServiceController.h"
#include "AuditLog.h"
#include "Engine.h"

namespace space2x::core {

ServiceController::ServiceController(IServiceManager& serviceManager,
                                     INetworkManager& networkManager,
                                     provider::ProviderRegistry& providerRegistry,
                                     AuditLog& auditLog)
    : m_serviceManager(serviceManager),
      m_networkManager(networkManager),
      m_providerRegistry(providerRegistry),
      m_auditLog(auditLog) {}

std::string ServiceController::resolveServiceName(const std::string& serviceId) const {
    auto provider = m_providerRegistry.find(serviceId);
    if (provider) {
        return provider->manifest().serviceNameForCurrentPlatform();
    }
    return serviceId;
}

Result<std::vector<ServiceInfo>> ServiceController::listServices() {
    // 1. Gather all registered providers
    auto providers = m_providerRegistry.all();
    std::vector<ServiceInfo> result;
    result.reserve(providers.size());

    for (const auto& prov : providers) {
        const auto& manifest = prov->manifest();
        ServiceInfo info;
        info.id = manifest.id;
        info.displayName = manifest.displayName;
        info.description = manifest.description;
        info.category = manifest.category;
        info.port = manifest.defaultPort;

        std::string sysName = manifest.serviceNameForCurrentPlatform();
        auto stateRes = m_serviceManager.getState(sysName);
        if (stateRes.isOk()) {
            info.state = stateRes.value();
        } else {
            info.state = ServiceState::Unknown;
        }

        result.push_back(std::move(info));
    }

    return Result<std::vector<ServiceInfo>>::ok(std::move(result));
}

Result<ServiceState> ServiceController::getServiceState(const std::string& serviceId) {
    std::string sysName = resolveServiceName(serviceId);
    return m_serviceManager.getState(sysName);
}

Result<void> ServiceController::startService(const std::string& serviceId) {
    auto provider = m_providerRegistry.find(serviceId);
    if (provider) {
        uint16_t port = provider->manifest().defaultPort;
        if (port > 0) {
            auto portRes = m_networkManager.isPortInUse(port);
            if (portRes.isOk() && portRes.value()) {
                m_auditLog.record("START_FAILED_PORT_COLLISION", serviceId, "FAILURE", "Port " + std::to_string(port) + " in use");
                return Result<void>::err(Error::make(
                    ErrorCode::PortInUse,
                    "Unable to start service '" + serviceId + "'.",
                    "Configured port " + std::to_string(port) + " is already bound by another process.",
                    "Stop the conflicting process or change the port in service configuration."
                ));
            }
        }
    }

    std::string sysName = resolveServiceName(serviceId);
    auto res = m_serviceManager.start(sysName);

    if (res.isOk()) {
        m_auditLog.record("START", serviceId, "SUCCESS");
    } else {
        m_auditLog.record("START", serviceId, "FAILURE", res.error().message);
    }

    return res;
}

Result<void> ServiceController::stopService(const std::string& serviceId) {
    std::string sysName = resolveServiceName(serviceId);
    auto res = m_serviceManager.stop(sysName);

    if (res.isOk()) {
        m_auditLog.record("STOP", serviceId, "SUCCESS");
    } else {
        m_auditLog.record("STOP", serviceId, "FAILURE", res.error().message);
    }

    return res;
}

Result<void> ServiceController::restartService(const std::string& serviceId) {
    std::string sysName = resolveServiceName(serviceId);
    auto res = m_serviceManager.restart(sysName);

    if (res.isOk()) {
        m_auditLog.record("RESTART", serviceId, "SUCCESS");
    } else {
        m_auditLog.record("RESTART", serviceId, "FAILURE", res.error().message);
    }

    return res;
}

Result<void> ServiceController::setAutoStart(const std::string& serviceId, bool enabled) {
    std::string sysName = resolveServiceName(serviceId);
    auto res = m_serviceManager.setAutoStart(sysName, enabled);

    if (res.isOk()) {
        m_auditLog.record("SET_AUTOSTART", serviceId, enabled ? "ENABLED" : "DISABLED");
    } else {
        m_auditLog.record("SET_AUTOSTART", serviceId, "FAILURE", res.error().message);
    }

    return res;
}

Result<void> ServiceController::performHealthCheck(const std::string& serviceId) {
    auto provider = m_providerRegistry.find(serviceId);
    if (!provider) {
        return Result<void>::err(Error::make(
            ErrorCode::ServiceNotFound,
            "Service provider not found for: " + serviceId
        ));
    }

    // Health check logic
    std::string sysName = provider->manifest().serviceNameForCurrentPlatform();
    auto stateRes = m_serviceManager.getState(sysName);
    if (!stateRes.isOk()) {
        return Result<void>::err(stateRes.error());
    }

    if (stateRes.value() != ServiceState::Running) {
        return Result<void>::err(Error::make(
            ErrorCode::ServiceNotRunning,
            "Service '" + serviceId + "' is not running (Current state: " + std::string(serviceStateToString(stateRes.value())) + ")."
        ));
    }

    return Result<void>::ok();
}

} // namespace space2x::core
