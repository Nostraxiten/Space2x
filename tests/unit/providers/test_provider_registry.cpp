#include <catch2/catch_test_macros.hpp>
#include <space2x/provider/IProvider.h>
#include <space2x/provider/ProviderRegistry.h>

#include <memory>
#include <string>
#include <vector>

using namespace space2x::core;
using namespace space2x::provider;

namespace {

class SampleProvider : public IProvider {
public:
    explicit SampleProvider(std::string id) {
        m_manifest.id = std::move(id);
        m_manifest.displayName = "Sample";
    }

    const ProviderManifest& manifest() const noexcept override {
        return m_manifest;
    }

    Result<ServiceState> detectState(const Engine&) const override {
        return Result<ServiceState>::ok(ServiceState::Stopped);
    }

private:
    ProviderManifest m_manifest;
};

} // anonymous namespace

TEST_CASE("ProviderRegistry registration and query operations", "[provider][registry]") {
    ProviderRegistry registry;

    SECTION("Register valid provider and find by ID") {
        auto prov = std::make_shared<SampleProvider>("redis");
        auto regRes = registry.registerProvider(prov);
        REQUIRE(regRes.isOk());

        auto found = registry.find("redis");
        REQUIRE(found != nullptr);
        REQUIRE(found->manifest().id == "redis");
    }

    SECTION("Reject duplicate provider IDs") {
        auto prov1 = std::make_shared<SampleProvider>("nginx");
        auto prov2 = std::make_shared<SampleProvider>("nginx");

        REQUIRE(registry.registerProvider(prov1).isOk());
        auto dupRes = registry.registerProvider(prov2);
        REQUIRE(dupRes.isErr());
    }

    SECTION("Reject null provider pointer") {
        auto nullRes = registry.registerProvider(nullptr);
        REQUIRE(nullRes.isErr());
    }

    SECTION("Find non-existent provider returns nullptr") {
        REQUIRE(registry.find("non_existent") == nullptr);
    }
}
