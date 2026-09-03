# Security Policy

Security is a primary concern in the design and implementation of Space2X. Because Space2X manages local services, network endpoints, and system configurations, maintaining strict security boundaries is critical.

---

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 0.1.x (Current Development) | :white_check_mark: |

---

## Reporting a Vulnerability

If you discover a potential security vulnerability in Space2X, please do not open a public issue. Instead, report it privately:

1. **Email:** Send an advisory report to `security@space2x.org` (or directly contact the core maintainers).
2. **Details:** Include a detailed description of the vulnerability, steps to reproduce, affected platform(s), and any potential mitigation strategies.
3. **Response Time:** Maintainers will acknowledge your report within 48 hours and provide a timeline for triage and remediation.

---

## Security Tenets

1. **Privilege Isolation:** Space2X avoids running full GUI processes with unrestricted administrative or root privileges. Elevated tasks (e.g., SCM manipulation, binding low-port sockets) require deliberate user elevation prompts.
2. **No Plaintext Secret Storage:** Passwords, API tokens, and database authentication strings are never written to unencrypted log files or persistent history databases.
3. **Explicit User Consent:** Destructive actions (stopping critical services, altering configurations, restoring database backups) require interactive confirmation.
4. **Network Exposure Protection:** Space2X never configures firewall rules or binds external listening sockets without explicit user specification.
