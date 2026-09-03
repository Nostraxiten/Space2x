#include <catch2/catch_test_macros.hpp>
#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <string>

using namespace space2x::core;

TEST_CASE("Result<T, E> basic success and failure semantics", "[core][result]") {
    SECTION("Result::ok holds value and reports isOk") {
        auto res = Result<int>::ok(42);
        REQUIRE(res.isOk());
        REQUIRE_FALSE(res.isErr());
        REQUIRE(static_cast<bool>(res));
        REQUIRE(res.value() == 42);
        REQUIRE(res.valueOr(100) == 42);
    }

    SECTION("Result::err holds error and reports isErr") {
        auto err = Error::make(ErrorCode::ServiceNotFound, "Service not found");
        auto res = Result<int>::err(err);
        REQUIRE(res.isErr());
        REQUIRE_FALSE(res.isOk());
        REQUIRE_FALSE(static_cast<bool>(res));
        REQUIRE(res.error().code == ErrorCode::ServiceNotFound);
        REQUIRE(res.valueOr(999) == 999);
    }
}

TEST_CASE("Result<T, E> monadic combinators", "[core][result]") {
    SECTION("map transforms value when Ok") {
        auto res = Result<int>::ok(10);
        auto mapped = res.map([](int x) { return std::to_string(x * 2); });
        REQUIRE(mapped.isOk());
        REQUIRE(mapped.value() == "20");
    }

    SECTION("map passes through error when Err") {
        auto res = Result<int>::err(Error::make(ErrorCode::PermissionDenied, "Denied"));
        auto mapped = res.map([](int x) { return std::to_string(x * 2); });
        REQUIRE(mapped.isErr());
        REQUIRE(mapped.error().code == ErrorCode::PermissionDenied);
    }

    SECTION("flatMap chains fallible operations") {
        auto divide = [](int a, int b) -> Result<int> {
            if (b == 0) return Result<int>::err(Error::make(ErrorCode::Unknown, "Divide by zero"));
            return Result<int>::ok(a / b);
        };

        auto res1 = Result<int>::ok(20).flatMap([&](int x) { return divide(x, 2); });
        REQUIRE(res1.isOk());
        REQUIRE(res1.value() == 10);

        auto res2 = Result<int>::ok(20).flatMap([&](int x) { return divide(x, 0); });
        REQUIRE(res2.isErr());
        REQUIRE(res2.error().message == "Divide by zero");
    }

    SECTION("mapError modifies the error type or message") {
        auto res = Result<int>::err(Error::make(ErrorCode::Timeout, "Operation timed out"));
        auto mapped = res.mapError([](const Error& e) {
            return Error::make(e.code, "Wrapped: " + e.message);
        });
        REQUIRE(mapped.isErr());
        REQUIRE(mapped.error().message == "Wrapped: Operation timed out");
    }
}

TEST_CASE("Result<void, E> specialization", "[core][result]") {
    SECTION("Result<void>::ok succeeds") {
        auto res = Result<void>::ok();
        REQUIRE(res.isOk());
        REQUIRE_FALSE(res.isErr());
        REQUIRE(static_cast<bool>(res));
    }

    SECTION("Result<void>::err fails") {
        auto res = Result<void>::err(Error::make(ErrorCode::PortInUse, "Port busy"));
        REQUIRE(res.isErr());
        REQUIRE(res.error().code == ErrorCode::PortInUse);
    }

    SECTION("Result<void> map and flatMap") {
        int sideEffect = 0;
        auto res = Result<void>::ok();
        auto mapped = res.map([&]() { sideEffect = 42; return 100; });
        REQUIRE(sideEffect == 42);
        REQUIRE(mapped.isOk());
        REQUIRE(mapped.value() == 100);
    }
}
