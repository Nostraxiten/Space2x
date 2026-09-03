#include "Engine.h"
#include <cassert>

#include "../platform/windows/WinNetworkManager.h"
#include "../platform/windows/WinPackageManager.h"
#include "../platform/windows/WinProcessManager.h"
#include "../platform/windows/WinServiceManager.h"
#include "../platform/windows/WinSystemMonitor.h"

#include "../platform/linux/LinuxNetworkManager.h"
#include "../platform/linux/LinuxPackageManager.h"
#include "../platform/linux/LinuxProcessManager.h"
#include "../platform/linux/LinuxServiceManager.h"
#include "../platform/linux/LinuxSystemMonitor.h"

#include "../providers/mysql/MySQLProvider.h"
#include "../providers/nginx/NginxProvider.h"
#include "../providers/openssh/OpenSSHProvider.h"
#include "../providers/postgresql/PostgreSQLProvider.h"
#include "../providers/redis/RedisProvider.h"

namespace space2x::core {

std::unique_ptr<Engine> Engine::create() {
#if defined(_WIN32)
    auto svcMgr = std::make_unique<platform::windows::WinServiceManager>();
    auto procMgr = std::make_unique<platform::windows::WinProcessManager>();
    auto sysMon = std::make_unique<platform::windows::WinSystemMonitor>();
    auto netMgr = std::make_unique<platform::windows::WinNetworkManager>();
    auto pkgMgr = std::make_unique<platform::windows::WinPackageManager>();
#else
    auto svcMgr = std::make_unique<platform::linux_os::LinuxServiceManager>();
    auto procMgr = std::make_unique<platform::linux_os::LinuxProcessManager>();
    auto sysMon = std::make_unique<platform::linux_os::LinuxSystemMonitor>();
    auto netMgr = std::make_unique<platform::linux_os::LinuxNetworkManager>();
    auto pkgMgr = std::make_unique<platform::linux_os::LinuxPackageManager>();
#endif

    auto engine = std::make_unique<Engine>(
        std::move(svcMgr),
        std::move(procMgr),
        std::move(sysMon),
        std::move(netMgr),
        std::move(pkgMgr)
    );

    engine->registerDefaultProviders();
    return engine;
}

Engine::Engine(std::unique_ptr<IServiceManager> serviceManager,
               std::unique_ptr<IProcessManager> processManager,
               std::unique_ptr<ISystemMonitor>  systemMonitor,
               std::unique_ptr<INetworkManager> networkManager,
               std::unique_ptr<IPackageManager> packageManager)
    : m_serviceManager(std::move(serviceManager)),
      m_processManager(std::move(processManager)),
      m_systemMonitor(std::move(systemMonitor)),
      m_networkManager(std::move(networkManager)),
      m_packageManager(std::move(packageManager)),
      m_serviceController(*m_serviceManager, *m_networkManager, m_providerRegistry, m_auditLog) {}

void Engine::registerDefaultProviders() {
    // All default providers have unique IDs — registration failures here are
    // programming errors, not runtime conditions. We assert in debug builds.
    auto reg = [this](std::shared_ptr<provider::IProvider> p) {
        auto res = m_providerRegistry.registerProvider(std::move(p));
        assert(res.isOk() && "Default provider registration failed — duplicate or null ID");
        (void)res;
    };

    reg(std::make_shared<providers::OpenSSHProvider>());
    reg(std::make_shared<providers::PostgreSQLProvider>());
    reg(std::make_shared<providers::MySQLProvider>());
    reg(std::make_shared<providers::NginxProvider>());
    reg(std::make_shared<providers::RedisProvider>());

    // Populate version store
    m_versionStore.registerServiceVersion({
        "openssh", "9.6.0", "9.6.0", {"9.5.0", "9.6.0"}, {}
    });
    m_versionStore.registerServiceVersion({
        "postgresql", "16.2", "16.2", {"15.6", "16.2"}, {"11.0", "12.0"}
    });
    m_versionStore.registerServiceVersion({
        "mysql", "8.0.36", "8.0.36", {"8.0.35", "8.0.36", "8.3.0"}, {"5.7.0"}
    });
    m_versionStore.registerServiceVersion({
        "nginx", "1.24.0", "1.24.0", {"1.22.0", "1.24.0", "1.25.4"}, {}
    });
    m_versionStore.registerServiceVersion({
        "redis", "7.2.4", "7.2.4", {"6.2.14", "7.0.15", "7.2.4"}, {"5.0.0"}
    });
}

} // namespace space2x::core
