#pragma once

#include <space2x/core/Error.h>
#include <space2x/core/INetworkManager.h>
#include <space2x/core/IServiceManager.h>
#include <space2x/core/Result.h>
#include <space2x/core/ServiceInfo.h>
#include <space2x/core/ServiceState.h>
#include <space2x/provider/ProviderRegistry.h>

namespace space2x::core {

class AuditLog;

class ServiceController {
public:
    ServiceController(IServiceManager& serviceManager,
                      INetworkManager& networkManager,
                      provider::ProviderRegistry& providerRegistry,
                      AuditLog& auditLog);
    ~ServiceController() = default;

    [[nodiscard]] Result<std::vector<ServiceInfo>> listServices();
    [[nodiscard]] Result<ServiceState>             getServiceState(const std::string& serviceId);
    Result<void>                                 startService(const std::string& serviceId);
    Result<void>                                 stopService(const std::string& serviceId);
    Result<void>                                 restartService(const std::string& serviceId);
    Result<void>                                 setAutoStart(const std::string& serviceId, bool enabled);
    Result<void>                                 performHealthCheck(const std::string& serviceId);

private:
    [[nodiscard]] std::string resolveServiceName(const std::string& serviceId) const;

    IServiceManager&            m_serviceManager;
    INetworkManager&            m_networkManager;
    provider::ProviderRegistry& m_providerRegistry;
    AuditLog&                   m_auditLog;
};

} // namespace space2x::core
