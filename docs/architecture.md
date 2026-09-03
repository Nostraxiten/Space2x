# Space2X Architecture Specification

## 1. Architectural Philosophy

Space2X is engineered with strict boundaries between domain logic, platform abstraction, and user interaction surfaces (GUI/CLI). The design prioritizes:

- **Modularity:** Core business rules are decoupled from concrete operating system APIs.
- **Robustness:** Zero reliance on exceptions for control flow. All fallible operations return typed `Result<T, Error>`.
- **Extensibility:** All service-specific logic is isolated into self-contained *Providers*.
- **Transparency:** No hidden system modifications; user confirmation precedes any state change.

---

## 2. Component Hierarchy

```
+-------------------------------------------------------------+
|                      Presentation Tier                      |
|   - space2x-ui (Qt 6 Desktop Application)                   |
|   - space2x-cli (Terminal Utility & Automation Engine)      |
+------------------------------+------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|                      Space2X Core Lib                       |
|   - Engine (Lifecycle & Dependency Injector)                |
|   - ServiceController (State Machine Transitions)          |
|   - ConfigManager (Delta Diffing, Atomic Backups, Rollback) |
|   - ProfileManager (Stack Provisioning & Resolution)        |
|   - AuditLog (SQLite-backed Immutable Event Ledger)         |
|   - VersionStore (Version Metadata & Upstream Registry)     |
+---------------+-----------------------------+---------------+
                |                             |
                v                             v
+-------------------------------+ +-------------------------------+
|     Platform Abstractions     | |       Provider Subsystem      |
|  - IServiceManager            | |  - ProviderRegistry           |
|  - IProcessManager            | |  - ProviderManifest           |
|  - INetworkManager            | |  - IProvider Interface        |
|  - ISystemMonitor             | |  - Concrete Providers:        |
|  - IPackageManager            | |    OpenSSH, PostgreSQL,       |
|                               | |    MySQL, Nginx, Redis        |
+---------------+---------------+ +---------------+---------------+
                |                                 |
        +-------+-------+                         |
        |               |                         |
        v               v                         |
+---------------+---------------+                 |
| Windows Native|  Linux Native |                 |
| - SCM         | - systemd     |<----------------+
| - ToolHelp32  | - /proc       |
| - IP Helper   | - Netlink     |
| - PDH         | - /proc/stat  |
+---------------+---------------+
```

---

## 3. Core Subsystems

### 3.1 Result and Error Type System

To ensure deterministic error handling across platform layers, Space2X utilizes a monadic `Result<T, E>` type.

```cpp
namespace space2x::core {

enum class ErrorCode {
    Success = 0,
    ServiceNotFound,
    ServiceNotRunning,
    ServiceAlreadyRunning,
    ServiceStartFailed,
    ServiceStopFailed,
    PermissionDenied,
    PortInUse,
    ConfigInvalid,
    ConfigFileNotFound,
    PlatformNotSupported,
    NetworkUnavailable,
    ProcessNotFound,
    Timeout,
    IoError,
    ParseError,
    Unknown
};

struct Error {
    ErrorCode            code;
    std::string          message;       // Human-readable summary
    std::string          detail;        // Diagnostic information
    std::string          suggestion;    // Actionable remediation steps
    std::optional<Error> cause;         // Upstream cause chain
};

} // namespace space2x::core
```

### 3.2 Service Controller & State Transition Machine

The `ServiceController` ensures services transition cleanly through documented lifecycle states:
- `NotInstalled`
- `Installed`
- `Starting`
- `Running`
- `Stopping`
- `Stopped`
- `Degraded`
- `Unknown`

Before executing state transitions (e.g. `start`), the controller:
1. Verifies prerequisites (executable exists, configuration validates).
2. Performs port availability collision checks via `INetworkManager`.
3. Requests state transition via the platform's `IServiceManager`.
4. Writes an immutable audit entry to `AuditLog`.

---

## 4. Platform Abstraction Layer

All platform-specific code resides in `src/platform/`. Public interfaces reside strictly in `include/space2x/core/`.

| Interface | Windows Implementation | Linux Implementation |
|---|---|---|
| `IServiceManager` | `WinServiceManager` (Windows SCM via `advapi32.dll`) | `LinuxServiceManager` (systemd D-Bus via `sd-bus`) |
| `IProcessManager` | `WinProcessManager` (`CreateToolhelp32Snapshot`) | `LinuxProcessManager` (`/proc/[pid]`) |
| `INetworkManager` | `WinNetworkManager` (IP Helper API `iphlpapi.dll`) | `LinuxNetworkManager` (`/proc/net`, Netlink sockets) |
| `ISystemMonitor` | `WinSystemMonitor` (`GetSystemTimes`, `GlobalMemoryStatusEx`) | `LinuxSystemMonitor` (`/proc/stat`, `/proc/meminfo`) |
| `IPackageManager` | `WinPackageManager` (`winget`, `choco`) | `LinuxPackageManager` (`apt`, `dnf`, `pacman`) |

---

## 5. Security & Isolation

- **Privilege Separation:** GUI and Core execute with unprivileged user context by default. Elevation is requested explicitly and transiently only when writing to privileged system locations or invoking SCM/Polkit APIs.
- **Audit Logging:** Every administrative action is stored in an SQLite audit database with timestamp, action type, target identifier, and user context.
