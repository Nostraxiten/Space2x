#include <catch2/catch_test_macros.hpp>
#include <space2x/core/AuditLog.h>
#include <space2x/core/INetworkManager.h>
#include <space2x/core/IServiceManager.h>
#include <space2x/core/ProfileManager.h>
#include <space2x/core/ServiceController.h>

#include <string>
#include <vector>

using namespace space2x::core;
using namespace space2x::provider;

namespace {

class DummyServiceManager : public IServiceManager {
public:
    Result<std::vector<ServiceInfo>> listServices() override { return Result<std::vector<ServiceInfo>>::ok({}); }
    Result<ServiceState> getState(const std::string&) override { return Result<ServiceState>::ok(ServiceState::Stopped); }
    Result<void> start(const std::string&) override { return Result<void>::ok(); }
    Result<void> stop(const std::string&) override { return Result<void>::ok(); }
    Result<void> restart(const std::string&) override { return Result<void>::ok(); }
    Result<void> setAutoStart(const std::string&, bool) override { return Result<void>::ok(); }
};

class DummyNetworkManager : public INetworkManager {
public:
    Result<std::vector<NetworkInterface>> getInterfaces() override { return Result<std::vector<NetworkInterface>>::ok({}); }
    Result<std::vector<ListeningSocket>> getListeningSockets() override { return Result<std::vector<ListeningSocket>>::ok({}); }
    Result<bool> isPortInUse(uint16_t, const std::string&) override { return Result<bool>::ok(false); }
};

} // anonymous namespace

TEST_CASE("ProfileManager registration and resolution", "[core][profile]") {
    ProfileManager profileMgr;

    ServiceProfile profile;
    profile.name = "Test Stack";
    profile.version = "1.0.0";
    profile.description = "Test stack description";
    profile.services = {
        {"nginx", "ensure_running", true},
        {"redis", "ensure_running", false}
    };

    profileMgr.registerProfile(profile);

    SECTION("List and retrieve registered profile") {
        auto list = profileMgr.listProfiles();
        REQUIRE(list.size() == 1);
        REQUIRE(list[0].name == "Test Stack");

        auto pRes = profileMgr.getProfile("Test Stack");
        REQUIRE(pRes.isOk());
        REQUIRE(pRes.value().services.size() == 2);
    }

    SECTION("Apply profile in dry-run mode") {
        DummyServiceManager svcMgr;
        DummyNetworkManager netMgr;
        ProviderRegistry registry;
        AuditLog auditLog("profile-test-audit.log");
        ServiceController controller(svcMgr, netMgr, registry, auditLog);

        auto applyRes = profileMgr.applyProfile("Test Stack", controller, true);
        REQUIRE(applyRes.isOk());
        REQUIRE(applyRes.value().size() == 2);
        REQUIRE(applyRes.value()[0].find("[DryRun]") != std::string::npos);
    }
}
