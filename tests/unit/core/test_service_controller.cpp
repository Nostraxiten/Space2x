#include <catch2/catch_test_macros.hpp>
#include <space2x/core/AuditLog.h>
#include <space2x/core/INetworkManager.h>
#include <space2x/core/IServiceManager.h>
#include <space2x/core/ServiceController.h>
#include <space2x/provider/IProvider.h>

using namespace space2x::core;
using namespace space2x::provider;

namespace {

class MockServiceManager : public IServiceManager {
public:
    Result<std::vector<ServiceInfo>> listServices() override {
        return Result<std::vector<ServiceInfo>>::ok({});
    }

    Result<ServiceState> getState(const std::string& serviceId) override {
        auto it = m_states.find(serviceId);
        if (it != m_states.end()) return Result<ServiceState>::ok(it->second);
        return Result<ServiceState>::ok(ServiceState::Stopped);
    }

    Result<void> start(const std::string& serviceId) override {
        m_states[serviceId] = ServiceState::Running;
        m_startCount++;
        return Result<void>::ok();
    }

    Result<void> stop(const std::string& serviceId) override {
        m_states[serviceId] = ServiceState::Stopped;
        m_stopCount++;
        return Result<void>::ok();
    }

    Result<void> restart(const std::string& serviceId) override {
        stop(serviceId);
        start(serviceId);
        return Result<void>::ok();
    }

    Result<void> setAutoStart(const std::string&, bool) override {
        return Result<void>::ok();
    }

    std::map<std::string, ServiceState> m_states;
    int m_startCount{0};
    int m_stopCount{0};
};

class MockNetworkManager : public INetworkManager {
public:
    Result<std::vector<NetworkInterface>> getInterfaces() override {
        return Result<std::vector<NetworkInterface>>::ok({});
    }
    Result<std::vector<ListeningSocket>> getListeningSockets() override {
        return Result<std::vector<ListeningSocket>>::ok({});
    }
    Result<bool> isPortInUse(uint16_t port, const std::string&) override {
        return Result<bool>::ok(m_usedPorts.contains(port));
    }

    std::set<uint16_t> m_usedPorts;
};

class MockProvider : public IProvider {
public:
    explicit MockProvider(ProviderManifest m) : m_manifest(std::move(m)) {}
    const ProviderManifest& manifest() const noexcept override { return m_manifest; }
    Result<ServiceState> detectState(const Engine&) const override {
        return Result<ServiceState>::ok(ServiceState::Stopped);
    }
private:
    ProviderManifest m_manifest;
};

} // anonymous namespace

TEST_CASE("ServiceController lifecycle operations and port safety", "[core][service_controller]") {
    MockServiceManager mockSvcMgr;
    MockNetworkManager mockNetMgr;
    ProviderRegistry registry;
    AuditLog auditLog("test-audit.log");

    ProviderManifest manifest;
    manifest.id = "mock-db";
    manifest.displayName = "Mock Database";
    manifest.defaultPort = 5432;
    manifest.serviceNames = {{"windows", "mock-db-svc"}, {"linux", "mock-db.service"}};

    registry.registerProvider(std::make_shared<MockProvider>(manifest));

    ServiceController controller(mockSvcMgr, mockNetMgr, registry, auditLog);

    SECTION("Normal start transitions state and logs audit") {
        auto startRes = controller.startService("mock-db");
        REQUIRE(startRes.isOk());
        REQUIRE(mockSvcMgr.m_startCount == 1);
    }

    SECTION("Start fails when port is already in use") {
        mockNetMgr.m_usedPorts.insert(5432);

        auto startRes = controller.startService("mock-db");
        REQUIRE(startRes.isErr());
        REQUIRE(startRes.error().code == ErrorCode::PortInUse);
        REQUIRE(mockSvcMgr.m_startCount == 0);
    }

    SECTION("Stop transitions state") {
        auto stopRes = controller.stopService("mock-db");
        REQUIRE(stopRes.isOk());
        REQUIRE(mockSvcMgr.m_stopCount == 1);
    }
}
