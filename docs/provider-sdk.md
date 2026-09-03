# Space2X Provider SDK Guide

## Overview

A **Provider** in Space2X encapsulates all metadata, health check strategies, configuration handling, and platform mappings for a specific software service (e.g., PostgreSQL, Nginx, Redis).

Providers are decoupled from the core engine. Space2X allows registering built-in static providers as well as custom third-party providers.

---

## 1. Provider Manifest (`manifest.json`)

Every provider defines a declarative `manifest.json` complying with the Space2X Provider JSON Schema:

```json
{
  "$schema": "https://space2x.org/schemas/provider-manifest.schema.json",
  "id": "nginx",
  "displayName": "Nginx HTTP & Reverse Proxy",
  "version": "1.24.0",
  "category": "web-server",
  "description": "High performance web server and reverse proxy",
  "supportedPlatforms": ["windows", "linux"],
  "supportedArchitectures": ["x86_64", "arm64"],
  "defaultPort": 80,
  "serviceNames": {
    "windows": "nginx",
    "linux": "nginx.service"
  },
  "configPaths": {
    "windows": ["C:\\nginx\\conf\\nginx.conf"],
    "linux": ["/etc/nginx/nginx.conf"]
  },
  "logPaths": {
    "windows": ["C:\\nginx\\logs\\error.log"],
    "linux": ["/var/log/nginx/error.log"]
  },
  "healthCheck": {
    "type": "tcp",
    "port": 80,
    "timeoutMs": 2000
  }
}
```

---

## 2. Implementing `IProvider` in C++

```cpp
#include <space2x/provider/IProvider.h>
#include <space2x/provider/ProviderManifest.h>

namespace space2x::providers {

class CustomProvider : public provider::IProvider {
public:
    explicit CustomProvider(provider::ProviderManifest manifest)
        : m_manifest(std::move(manifest)) {}

    const provider::ProviderManifest& manifest() const noexcept override {
        return m_manifest;
    }

    core::Result<core::ServiceState> detectState(core::IServiceManager& serviceManager) const override {
        // Custom detection logic or delegation to serviceManager
        return serviceManager.getState(m_manifest.serviceNameForCurrentPlatform());
    }

    core::Result<void> performHealthCheck(core::IServiceManager& serviceManager) const override {
        // Optional custom health validation
        return core::Result<void>::ok();
    }

private:
    provider::ProviderManifest m_manifest;
};

} // namespace space2x::providers
```

---

## 3. Registering a Provider

Providers are registered into `ProviderRegistry`:

```cpp
auto& registry = engine->providerRegistry();
registry.registerProvider(std::make_shared<CustomProvider>(manifest));
```
