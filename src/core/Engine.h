#pragma once

#include <memory>
#include <space2x/core/INetworkManager.h>
#include <space2x/core/IPackageManager.h>
#include <space2x/core/IProcessManager.h>
#include <space2x/core/IServiceManager.h>
#include <space2x/core/ISystemMonitor.h>
#include <space2x/provider/ProviderRegistry.h>

#include "AuditLog.h"
#include "ConfigManager.h"
#include "ProfileManager.h"
#include "ServiceController.h"
#include "VersionStore.h"

namespace space2x::core {

class Engine {
public:
    static std::unique_ptr<Engine> create();

    Engine(std::unique_ptr<IServiceManager> serviceManager,
           std::unique_ptr<IProcessManager> processManager,
           std::unique_ptr<ISystemMonitor>  systemMonitor,
           std::unique_ptr<INetworkManager> networkManager,
           std::unique_ptr<IPackageManager> packageManager);

    ~Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    [[nodiscard]] IServiceManager&            serviceManager() noexcept { return *m_serviceManager; }
    [[nodiscard]] const IServiceManager&      serviceManager() const noexcept { return *m_serviceManager; }

    [[nodiscard]] IProcessManager&            processManager() noexcept { return *m_processManager; }
    [[nodiscard]] const IProcessManager&      processManager() const noexcept { return *m_processManager; }

    [[nodiscard]] ISystemMonitor&             systemMonitor() noexcept { return *m_systemMonitor; }
    [[nodiscard]] const ISystemMonitor&       systemMonitor() const noexcept { return *m_systemMonitor; }

    [[nodiscard]] INetworkManager&            networkManager() noexcept { return *m_networkManager; }
    [[nodiscard]] const INetworkManager&      networkManager() const noexcept { return *m_networkManager; }

    [[nodiscard]] IPackageManager&            packageManager() noexcept { return *m_packageManager; }
    [[nodiscard]] const IPackageManager&      packageManager() const noexcept { return *m_packageManager; }

    [[nodiscard]] provider::ProviderRegistry& providerRegistry() noexcept { return m_providerRegistry; }
    [[nodiscard]] const provider::ProviderRegistry& providerRegistry() const noexcept { return m_providerRegistry; }

    [[nodiscard]] ServiceController&          serviceController() noexcept { return m_serviceController; }
    [[nodiscard]] const ServiceController&    serviceController() const noexcept { return m_serviceController; }

    [[nodiscard]] ConfigManager&              configManager() noexcept { return m_configManager; }
    [[nodiscard]] const ConfigManager&        configManager() const noexcept { return m_configManager; }

    [[nodiscard]] ProfileManager&             profileManager() noexcept { return m_profileManager; }
    [[nodiscard]] const ProfileManager&       profileManager() const noexcept { return m_profileManager; }

    [[nodiscard]] AuditLog&                   auditLog() noexcept { return m_auditLog; }
    [[nodiscard]] const AuditLog&             auditLog() const noexcept { return m_auditLog; }

    [[nodiscard]] VersionStore&               versionStore() noexcept { return m_versionStore; }
    [[nodiscard]] const VersionStore&         versionStore() const noexcept { return m_versionStore; }

private:
    void registerDefaultProviders();

    std::unique_ptr<IServiceManager> m_serviceManager;
    std::unique_ptr<IProcessManager> m_processManager;
    std::unique_ptr<ISystemMonitor>  m_systemMonitor;
    std::unique_ptr<INetworkManager> m_networkManager;
    std::unique_ptr<IPackageManager> m_packageManager;

    provider::ProviderRegistry       m_providerRegistry;
    AuditLog                         m_auditLog;
    VersionStore                     m_versionStore;
    ConfigManager                    m_configManager;
    ProfileManager                   m_profileManager;
    ServiceController                m_serviceController;
};

} // namespace space2x::core
