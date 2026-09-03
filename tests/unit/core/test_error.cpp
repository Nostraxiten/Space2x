#include <catch2/catch_test_macros.hpp>
#include <space2x/core/Error.h>

using namespace space2x::core;

TEST_CASE("Error creation and diagnostic formatting", "[core][error]") {
    SECTION("Basic error properties") {
        auto err = Error::make(
            ErrorCode::PortInUse,
            "Unable to start PostgreSQL",
            "Port 5432 is already bound by PID 4812",
            "Stop the conflicting service or configure a different port"
        );

        REQUIRE(err.code == ErrorCode::PortInUse);
        REQUIRE(err.message == "Unable to start PostgreSQL");
        REQUIRE(err.detail == "Port 5432 is already bound by PID 4812");
        REQUIRE(err.suggestion == "Stop the conflicting service or configure a different port");
        REQUIRE(err.cause == nullptr);

        std::string str = err.toString();
        REQUIRE(str.find("[PortInUse]") != std::string::npos);
        REQUIRE(str.find("Detail: Port 5432 is already bound by PID 4812") != std::string::npos);
        REQUIRE(str.find("Suggestion: Stop the conflicting service") != std::string::npos);
    }

    SECTION("Error cause chaining") {
        auto rootCause = std::make_shared<Error>(Error::make(
            ErrorCode::PermissionDenied,
            "Access denied to SCM handle",
            "Win32 error 5"
        ));

        auto topError = Error::make(
            ErrorCode::ServiceStartFailed,
            "Failed to start service Nginx",
            "Underlying platform call failed",
            "Run Space2X as Administrator",
            rootCause
        );

        REQUIRE(topError.cause != nullptr);
        REQUIRE(topError.cause->code == ErrorCode::PermissionDenied);

        std::string formatted = topError.toString();
        REQUIRE(formatted.find("Caused by:") != std::string::npos);
        REQUIRE(formatted.find("[PermissionDenied]") != std::string::npos);
    }
}
