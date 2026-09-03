# Contributing to Space2X

Thank you for your interest in contributing to Space2X.

Space2X is a serious, production-focused open-source project. We prioritize code quality, correctness, security, architectural integrity, and cross-platform reliability over rapid feature accumulation.

---

## Code of Conduct

All contributors and maintainers are expected to maintain professional, constructive, and respectful interactions at all times.

---

## Architectural Principles

Before contributing code, please understand our core principles:

1. **Clean Abstractions:** Never leak platform-specific headers (`windows.h`, `systemd/sd-bus.h`, etc.) into `include/space2x/core/` or public API headers.
2. **Result-Oriented Error Handling:** Never throw naked C++ exceptions for standard operational failures (service missing, port bound, permission denied). Always return `space2x::core::Result<T, space2x::core::Error>`.
3. **No Fake Implementations:** If a feature or platform backend cannot yet be implemented properly, implement the interface method returning `ErrorCode::PlatformNotSupported` or `ErrorCode::NotImplemented` with clear diagnostic context. Do not write dummy mocks that give false positives.
4. **Tested Changes:** Every bugfix or new core feature must be accompanied by unit tests in `tests/unit/`.
5. **Modern C++20:** Write clean, safe, modern C++20. Prefer value semantics, RAII, `std::string_view`, `std::span`, and smart pointers.

---

## Development Workflow

### 1. Setting Up the Environment

Ensure you have:
- A C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022 v143+)
- CMake 3.22+ and Ninja
- Qt 6 (6.4+) installed and registered in `CMAKE_PREFIX_PATH`

### 2. Building and Testing

```bash
# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DSPACE2X_BUILD_TESTS=ON

# Compile
cmake --build build

# Run unit tests
ctest --test-dir build --output-on-failure
```

### 3. Code Style

- Use `clang-format` with the project configuration (`.clang-format`).
- Follow standard camelCase for member methods and PascalCase for types.
- Member variables should be prefixed with `m_` or named cleanly in private structs.
- Use explicit `#include` statements; do not rely on transitive includes.

### 4. Submitting Pull Requests

1. Fork the repository and create a descriptive feature branch (e.g. `feat/network-netlink-monitor` or `fix/scm-handle-leak`).
2. Ensure all existing and new unit tests pass cleanly.
3. Verify that the build succeeds with zero compiler warnings (`-Wall -Wextra -Werror` / `/W4 /WX`).
4. Write clear commit messages following Conventional Commits format (`feat:`, `fix:`, `docs:`, `refactor:`, `test:`).
5. Open a Pull Request referencing any relevant issues.
