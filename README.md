# Space2X

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Platform Support](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-brightgreen.svg)]()

**Space2X** is an open-source, cross-platform local infrastructure and service management platform for Windows and Linux systems. It provides a unified, reliable control plane for discovering, provisioning, configuring, starting, stopping, monitoring, and profiling local development infrastructure.

Designed as a modern, production-grade alternative to legacy stacks such as XAMPP or ad-hoc process management scripts, Space2X unifies OS-native service mechanisms behind a robust, strictly typed C++20 core architecture without abstracting away platform control or system transparency.

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Architecture](#architecture)
  - [Core Abstraction Layer](#core-abstraction-layer)
  - [Provider Subsystem](#provider-subsystem)
  - [Error Handling & Type Safety](#error-handling--type-safety)
- [Supported Platforms & Ecosystem](#supported-platforms--ecosystem)
- [Standalone Quickstart (Windows)](#standalone-quickstart-windows)
- [Building from Source](#building-from-source)
  - [Prerequisites](#prerequisites)
  - [Build Instructions](#build-instructions)
  - [CMake Configuration Options](#cmake-configuration-options)
- [Usage](#usage)
  - [Graphical Interface (GUI)](#graphical-interface-gui)
  - [Command-Line Interface (CLI)](#command-line-interface-cli)
- [Security Model](#security-model)
- [Service Profiles](#service-profiles)
- [Audit Logging & History](#audit-logging--history)
- [Roadmap](#roadmap)
- [Binary Releases Policy](#binary-releases-policy)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

Local service orchestration typically requires navigating disparate, platform-specific tooling: Windows Service Control Manager (SCM), Windows Management Instrumentation (WMI), Linux `systemd`, sysvinit scripts, package managers (`winget`, `apt`, `dnf`, `pacman`), and heterogeneous service-specific configuration formats.

Space2X resolves this fragmentation by establishing an extensible, non-invasive management layer. It does not replace the host operating system's native process supervisors; instead, it interfaces directly with platform APIs to discover and orchestrate services cleanly.

```
                      +---------------------------------------+
                      |         Space2X User Surfaces         |
                      |  [ Qt 6 Desktop GUI ]   [ Native CLI] |
                      +-------------------+-------------------+
                                          |
                                          v
                      +---------------------------------------+
                      |             Space2X Core              |
                      |  - Engine Orchestrator                |
                      |  - ServiceController & AuditLog       |
                      |  - ConfigManager & ProfileManager     |
                      |  - ProviderRegistry                   |
                      +-------------------+-------------------+
                                          |
                        +-----------------+-----------------+
                        |                                   |
                        v                                   v
        +-------------------------------+   +-------------------------------+
        |      Platform Abstraction     |   |       Provider Subsystem      |
        |  - IServiceManager (SCM/DBus) |   |  - OpenSSH, PostgreSQL        |
        |  - IProcessManager (Win32/Proc|   |  - MySQL, MariaDB             |
        |  - INetworkManager (IPHelper) |   |  - Nginx, Redis, Apache       |
        |  - ISystemMonitor (PDH/Proc)  |   |  - Custom Third-Party SDK     |
        +-------------------------------+   +-------------------------------+
```

---

## Key Features

- **Cross-Platform Parity:** Consistent service management semantics across Windows 10/11 and standard Linux distributions (Ubuntu, Debian, Fedora, Arch Linux).
- **Extensible Provider Architecture:** Decoupled service providers defined through strict C++ interfaces and declarative JSON schemas.
- **Strict Error Modeling:** All operations return monadic `Result<T, Error>` values with actionable diagnostic feedback, root-cause chains, and suggested remediation steps.
- **Config Lifecycle Management:** Validation, structured diff calculation, preview confirmation, and automatic pre-modification snapshots for rollbacks.
- **Task & Health Telemetry:** Lightweight monitoring of CPU, physical memory, paging/swap utilization, TCP listening sockets, and process trees.
- **Reproducible Profiles:** Group related services (e.g., "Web Development", "Database Cluster") into version-controlled JSON profile manifests.
- **Security-First Tenets:** Zero automated exposure of listening ports to public interfaces, strict local audit trails, no plaintext secret logging, and granular privilege escalation boundaries.

---

## Architecture

### Core Abstraction Layer

The `space2x-core` library encapsulates business logic without direct dependencies on GUI frameworks or concrete OS headers. Operating system interactions are mediated through abstract interface contracts:

- `IServiceManager`: Queries, starts, stops, restarts, and controls auto-start registration of OS services.
- `IProcessManager`: Enumerates system processes, reads resource footprints, and handles process termination.
- `INetworkManager`: Maps network interfaces, IPv4/IPv6 addresses, default gateways, and active listening ports to corresponding process IDs.
- `ISystemMonitor`: Collects host-wide performance telemetry (CPU time deltas, memory usage, swap).
- `IPackageManager`: Detects upstream distribution channels and package installation states.

### Provider Subsystem

Services are never hardcoded into the core engine. Each supported service is encapsulated within a provider module accompanied by a `manifest.json` metadata document.

A provider declares:
- Canonical service identity, category, and display metadata.
- Supported platform matrix and architecture constraints.
- Default ports, configuration file paths, and log locations.
- Health check strategies (TCP handshake, UNIX domain socket ping, HTTP endpoint check).
- Operating system service identifiers (e.g., `postgresql-x64-16` on Windows vs. `postgresql.service` on Linux).

### Error Handling & Type Safety

Space2X avoids naked C++ exceptions for domain-level failure paths. All fallible operations return `space2x::core::Result<T, space2x::core::Error>`.

Errors include structured codes (`ErrorCode`), descriptive human-readable context, technical log details, and actionable suggestions:

```cpp
auto result = engine->serviceController().startService("postgresql");
if (!result.isOk()) {
    const auto& err = result.error();
    // Emits structured diagnostics:
    // Code: PortInUse
    // Message: Unable to start PostgreSQL service.
    // Detail: Local TCP socket 5432 is already bound by PID 4812.
    // Suggestion: Stop conflicting service on port 5432 or update PostgreSQL config.
}
```

---

## Supported Platforms & Ecosystem

| Platform | Service Backend | Process Backend | Network Backend | Telemetry Backend |
|---|---|---|---|---|
| **Windows 10 / 11 / Server** | Windows SCM (Advapi32) | Toolhelp32 / Win32 API | IP Helper (`iphlpapi.dll`) | PDH / System Performance Counters |
| **Linux (systemd-based)** | systemd D-Bus (`sd-bus`) | `/proc` filesystem | Netlink / `/proc/net` | `/proc/stat`, `/proc/meminfo` |

### Core Provider Catalog

- **Web & Reverse Proxy:** Nginx, Apache HTTP Server
- **Relational Databases:** PostgreSQL, MySQL, MariaDB
- **In-Memory Caching:** Redis
- **Remote Access:** OpenSSH (`sshd`)
- **Runtime Engines:** PHP FastCGI (FPM)

---

> [!WARNING]
> ### ⚠️ EXPERIMENTAL PROJECT & LEARNING DISCLAIMER
>
> **THIS IS A BRAND NEW PROJECT. I DO NOT HAVE ADVANCED KNOWLEDGE OF C++. DO NOT USE THIS IN PROFESSIONAL OR PRODUCTION ENVIRONMENTS.**
>
> Space2X is an experimental hobby project built for learning and exploration. It **has bugs, limitations, and may crash or behave unexpectedly**. It is not intended for mission-critical tasks. Feedback, bug reports, and pull requests from more experienced developers are highly appreciated!

---

## Standalone Quickstart (Windows)

For Windows users who want to test Space2X without compiling or configuring dependencies:
- Download the single self-contained executable: [`executable/Space2X.exe`](executable/Space2X.exe).
- Double-click `Space2X.exe` — it loads all runtime components, assets, and the Qt graphical interface in a single portable binary without requiring manual DLL installation or opening background terminal windows.

---

## Building from Source

### Prerequisites

- **C++ Compiler:** Supporting C++20 standard:
  - GCC 11+ (Linux)
  - Clang 14+ (Linux / macOS)
  - Microsoft Visual C++ 2022 (v143+) (Windows)
- **Build System:** CMake 3.22 or higher, Ninja (recommended) or MSBuild.
- **GUI Dependencies:** Qt 6 (6.4+) including `QtCore`, `QtGui`, `QtWidgets`, `QtNetwork`.
- **System Libraries:**
  - Linux: `libsystemd-dev` (or systemd D-Bus headers).
  - Windows: Windows SDK 10.0.19041.0+.

### Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/space2x/space2x.git
   cd space2x
   ```

2. **Configure with CMake:**
   ```bash
   cmake -B build -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DSPACE2X_BUILD_UI=ON \
     -DSPACE2X_BUILD_CLI=ON \
     -DSPACE2X_BUILD_TESTS=ON
   ```

3. **Compile:**
   ```bash
   cmake --build build --config Release
   ```

4. **Execute Test Suite:**
   ```bash
   ctest --test-dir build --output-on-failure
   ```

### CMake Configuration Options

| CMake Option | Default | Description |
|---|---|---|
| `SPACE2X_BUILD_UI` | `ON` | Compiles the Qt 6 graphical desktop application (`space2x-ui`). |
| `SPACE2X_BUILD_CLI` | `ON` | Compiles the standalone terminal utility (`space2x-cli`). |
| `SPACE2X_BUILD_TESTS` | `ON` | Compiles the Catch2 unit and contract test suites. |
| `SPACE2X_INTEGRATION_TESTS` | `OFF` | Enables integration tests requiring live OS service interaction. |
| `SPACE2X_ENABLE_WARNINGS_AS_ERRORS` | `ON` | Enforces strict compiler warnings (`-Werror` / `/WX`). |

---

## Usage

### Graphical Interface (GUI)

Launch the `space2x-ui` binary. The interface contains structured views:

1. **Dashboard:** Global health metrics, real-time CPU/memory graphs, and rapid-access service toggles.
2. **Services:** Complete catalog of registered and discovered services with status badges (`Running`, `Stopped`, `Degraded`, `Not Installed`).
3. **Databases:** Local instance detection, database listing, and credential/permission inspection.
4. **Processes:** Filterable process explorer displaying PID, resident memory, CPU %, and linked ports.
5. **Network:** Active network interfaces, local IPs, gateways, and listening socket bindings.
6. **Profiles:** Environment templates to activate multi-service stacks with one click.
7. **Logs & History:** Structured audit events and streamed service log tails.

### Command-Line Interface (CLI)

The `space2x-cli` tool exposes full management functionality for headless environments and scripts:

```bash
# Query service status
space2x services list
space2x services status postgresql

# Control services
space2x services start nginx
space2x services stop nginx
space2x services restart redis

# Apply service profile
space2x profiles list
space2x profiles apply "Web Development"

# Inspect system and network diagnostics
space2x system info
space2x network status

# Emit machine-readable JSON for automation
space2x --json services list
```

---

## Security Model

1. **Explicit Privilege Escalation:** Administrative operations (modifying system services, editing protected config paths) do not silently trigger background root commands. Elevation prompts are surfaced explicitly via platform facilities (UAC on Windows, Polkit on Linux).
2. **Safe Configuration Modification:** Space2X validates configurations prior to application, generates an atomic backup of target files, and presents a visual difference preview requiring user confirmation.
3. **Network Isolation:** Space2X never modifies host firewall configurations or opens external NAT forwarding rules without interactive confirmation.
4. **Zero Plaintext Secret Persistence:** Application logs, audit records, and serialized profiles never retain database passwords or credentials in plaintext.

---

## Service Profiles

Profiles allow reproducible definition of local development stacks. Example profile (`resources/profiles/web-development.json`):

```json
{
  "name": "Web Development",
  "version": "1.0.0",
  "description": "Standard LEMP stack with in-memory cache",
  "services": [
    { "id": "nginx", "action": "ensure_running", "autoStart": true },
    { "id": "mysql", "action": "ensure_running", "autoStart": false },
    { "id": "redis", "action": "ensure_running", "autoStart": false }
  ]
}
```

---

## Audit Logging & History

Every state-altering operation is logged locally in an SQLite-backed audit journal (`audit.db`) located in the user's configuration directory. Recorded fields include timestamp, initiator, target service, operation, exit outcome, and error codes.

---

## Roadmap

- **Phase 1 (Current):** Foundational architecture, C++20 core library, platform abstraction layer (Windows SCM / Linux D-Bus), system telemetry, base CLI, and core Qt6 GUI.
- **Phase 2:** Complete provider catalog (OpenSSH, PostgreSQL, MySQL/MariaDB, Nginx, Redis) and configuration parsers.
- **Phase 3:** Interactive configuration editor with delta preview, live log viewer, and profile synchronization.
- **Phase 4:** Local database administrative inspection tools, network connection mapper, and automated health checks.
- **Phase 5:** Third-party Provider SDK, out-of-process dynamic provider plugins, and cross-platform packaging (MSI/NSIS, AppImage, DEB, RPM).

---

## Binary Releases Policy

Precompiled binaries published in GitHub Releases are provided solely for user convenience and correspond to specific signed Git tags.

Building Space2X from source is recommended whenever possible to verify reproducibility and inspect dependencies against your local security policies. The source code repository is the single canonical reference implementation.

---

## Contributing

Contributions are welcomed. Please review [CONTRIBUTING.md](CONTRIBUTING.md) and [SECURITY.md](SECURITY.md) before submitting issues or pull requests.

---

## License

Space2X is licensed under the [Apache License, Version 2.0](LICENSE).
