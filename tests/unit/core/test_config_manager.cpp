#include <catch2/catch_test_macros.hpp>
#include <space2x/core/ConfigManager.h>
#include <filesystem>
#include <fstream>

using namespace space2x::core;
namespace fs = std::filesystem;

TEST_CASE("ConfigManager delta, backup, and rollback lifecycle", "[core][config]") {
    std::string testFile = "test_sample_config.conf";

    // Setup temporary test file
    {
        std::ofstream out(testFile);
        out << "port = 5432\nmax_connections = 100\n";
    }

    ConfigManager configMgr;

    SECTION("Read configuration file") {
        auto res = configMgr.readFile(testFile);
        REQUIRE(res.isOk());
        REQUIRE(res.value().find("port = 5432") != std::string::npos);
    }

    SECTION("Compute delta and preview") {
        std::vector<ConfigDeltaItem> items = {
            {"port", "port = 5432", "port = 5433"}
        };
        auto deltaRes = configMgr.computeDelta(testFile, items);
        REQUIRE(deltaRes.isOk());
        REQUIRE(deltaRes.value().hasChanges());
        REQUIRE(deltaRes.value().rawPreview.find("port = 5432 -> port = 5433") != std::string::npos);
    }

    SECTION("Apply delta creates backup and updates file content") {
        std::vector<ConfigDeltaItem> items = {
            {"port", "port = 5432", "port = 5433"}
        };
        auto deltaRes = configMgr.computeDelta(testFile, items);
        REQUIRE(deltaRes.isOk());

        auto applyRes = configMgr.applyDelta(deltaRes.value(), true);
        REQUIRE(applyRes.isOk());

        auto updatedContent = configMgr.readFile(testFile);
        REQUIRE(updatedContent.isOk());
        REQUIRE(updatedContent.value().find("port = 5433") != std::string::npos);
    }

    // Cleanup
    if (fs::exists(testFile)) {
        fs::remove(testFile);
    }
}
