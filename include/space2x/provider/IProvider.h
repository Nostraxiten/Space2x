#pragma once

#include "ProviderManifest.h"
#include <space2x/core/ConfigDelta.h>
#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <space2x/core/ServiceState.h>

namespace space2x::core {
class Engine;
}

namespace space2x::provider {

class IProvider {
public:
    virtual ~IProvider() = default;

    [[nodiscard]] virtual const ProviderManifest& manifest() const noexcept = 0;

    [[nodiscard]] virtual core::Result<core::ServiceState> detectState(const core::Engine& engine) const = 0;

    [[nodiscard]] virtual core::Result<void> performHealthCheck(const core::Engine& engine) const {
        (void)engine;
        return core::Result<void>::ok();
    }

    [[nodiscard]] virtual core::Result<void> install(const core::Engine& engine) const {
        (void)engine;
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Automated installation is not implemented for this provider."
        ));
    }

    [[nodiscard]] virtual core::Result<void> uninstall(const core::Engine& engine) const {
        (void)engine;
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Automated uninstallation is not implemented for this provider."
        ));
    }

    [[nodiscard]] virtual core::Result<void> configure(const core::Engine& engine, const core::ConfigDelta& delta) const {
        (void)engine;
        (void)delta;
        return core::Result<void>::err(core::Error::make(
            core::ErrorCode::NotImplemented,
            "Configuration management is not implemented for this provider."
        ));
    }
};

} // namespace space2x::provider
