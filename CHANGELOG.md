# Changelog

All notable changes to the Space2X project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased] - 0.1.0-alpha

### Added
- Core modular architecture with pure abstract interfaces (`IServiceManager`, `IProcessManager`, `ISystemMonitor`, `INetworkManager`, `IPackageManager`).
- Typed error handling system (`Error`, `ErrorCode`, and monadic `Result<T, E>`).
- Cross-platform engine orchestration (`Engine`, `ServiceController`, `ConfigManager`, `ProfileManager`, `AuditLog`, `VersionStore`).
- Windows platform implementations (Windows SCM service management, Toolhelp32 process inspection, PDH telemetry).
- Linux platform implementations (systemd D-Bus service management, `/proc` process & telemetry inspection).
- Provider subsystem with JSON manifest schema and static registration (`OpenSSH`, `PostgreSQL`, `MySQL`, `Nginx`, `Redis`).
- Reusable service profile manager with declarative schema validation.
- SQLite-backed audit log tracking all service lifecycle events.
- Standalone command-line interface (`space2x-cli`) supporting interactive and JSON output modes.
- Qt 6 desktop user interface (`space2x-ui`) with Dashboard, Services, Processes, Network, and Settings views.
- Comprehensive unit test suite with Catch2.
