#pragma once

#include "ProviderManifest.h"
#include <space2x/core/ConfigDelta.h>
#include <space2x/core/Error.h>
#include <space2x/core/IServiceManager.h>
#include <space2x/core/Result.h>
#include <space2x/core/ServiceState.h>

namespace space2x::provider {

/**
 * Base interface for all service and infrastructure providers.
 *
 * A provider encapsulates knowledge about a specific service (e.g., Redis,
 * PostgreSQL, Nginx): its system service name(s), config paths, health check
 * semantics, and version information.
 *
 * Providers are intentionally decoupled from the Engine. They receive only
 * the minimal collaborator they need (IServiceManager) so they can be
 * instantiated and tested without a full Engine context.
 */
class IProvider {
public:
    virtual ~IProvider() = default;

    /// Returns the provider's static manifest (id, display name, ports, paths, etc.)
    [[nodiscard]] virtual const ProviderManifest& manifest() const noexcept = 0;

    /**
     * Queries the live state of the underlying OS service via the service manager.
     * Must not perform I/O other than the service state query.
     */
    [[nodiscard]] virtual core::Result<core::ServiceState>
        detectState(core::IServiceManager& serviceManager) const = 0;

    /**
     * Performs a provider-specific health check. The default implementation
     * delegates to detectState and requires Running state. Override for
     * richer checks (e.g., TCP ping, protocol handshake).
     */
    [[nodiscard]] virtual core::Result<void>
        performHealthCheck(core::IServiceManager& serviceManager) const {
        auto stateRes = detectState(serviceManager);
        if (!stateRes.isOk()) {
            return core::Result<void>::err(stateRes.error());
        }
        if (stateRes.value() != core::ServiceState::Running) {
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::ServiceNotRunning,
                "Service '" + manifest().id + "' is not in a running state.",
                "Current state: " + std::string(core::serviceStateToString(stateRes.value()))
            ));
        }
        return core::Result<void>::ok();
    }

    [[nodiscard]] virtual core::Result<void>
        install(core::IServiceManager& /*serviceManager*/) const {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Automated installation is not implemented for this provider."
        ));
    }

    [[nodiscard]] virtual core::Result<void>
        uninstall(core::IServiceManager& /*serviceManager*/) const {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Automated uninstallation is not implemented for this provider."
        ));
    }

    [[nodiscard]] virtual core::Result<void>
        configure(core::IServiceManager& /*serviceManager*/,
                  const core::ConfigDelta& /*delta*/) const {
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Configuration management is not implemented for this provider."
        ));
    }
};

} // namespace space2x::provider
