#pragma once

#include <space2x/provider/IProvider.h>
#include <space2x/provider/ProviderManifest.h>

namespace space2x::providers {

class NginxProvider : public provider::IProvider {
public:
    NginxProvider();
    explicit NginxProvider(provider::ProviderManifest manifest);

    [[nodiscard]] const provider::ProviderManifest& manifest() const noexcept override;
    [[nodiscard]] core::Result<core::ServiceState> detectState(const core::Engine& engine) const override;
    [[nodiscard]] core::Result<void> performHealthCheck(const core::Engine& engine) const override;

private:
    provider::ProviderManifest m_manifest;
};

} // namespace space2x::providers
