# Space2X Security Model & Trust Boundaries

## 1. Principles of Operation

Space2X manages services and configurations that directly affect host system stability and security. As such, it adheres to four fundamental security tenets:

1. **Least Privilege by Default:** The primary user interfaces (GUI and CLI) run unprivileged. Administrative capabilities are partitioned and requested on-demand.
2. **Deterministic & Explicit Actions:** No silent network or firewall modifications. All operations that alter state require explicit confirmation.
3. **Atomic & Rollback-Ready State Changes:** Configuration file edits always produce pre-modification snapshots.
4. **Zero Unencrypted Secret Retention:** Passwords and tokens are kept in volatile memory only as long as necessary and never written to audit databases or plain log streams.

---

## 2. Platform Privilege Boundaries

### 2.1 Windows
- Unprivileged users can query service status, inspect processes, and view system metrics.
- Service start/stop/restart and configuration modifications to `%ProgramFiles%` trigger Windows UAC elevation.
- Elevation is conducted via child worker processes (`space2x-worker.exe` / `runas`) rather than keeping the entire GUI elevated.

### 2.2 Linux
- Direct SCM operations via systemd D-Bus interface utilize `polkit` authorization mechanisms.
- File system modifications for system services in `/etc/` utilize standard privilege broker mechanisms.
- Headless CLI operations leverage `sudo` with granular command restrictions where appropriate.

---

## 3. Network Protection

- Space2X binds management endpoints (if enabled) strictly to `127.0.0.1` / `::1`.
- Services managed by Space2X default to local socket listening configurations where supported.
- Diagnostic port binding checks warn users whenever a service is configured to bind to `0.0.0.0` (all interfaces).
