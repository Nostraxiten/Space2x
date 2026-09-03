#pragma once

#include "IProvider.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace space2x::provider {

class ProviderRegistry {
public:
    ProviderRegistry() = default;
    ~ProviderRegistry() = default;

    ProviderRegistry(const ProviderRegistry&) = delete;
    ProviderRegistry& operator=(const ProviderRegistry&) = delete;

    [[nodiscard]] core::Result<void> registerProvider(std::shared_ptr<IProvider> provider) {
        if (!provider) {
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::Unknown,
                "Cannot register null provider pointer."
            ));
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        const auto& id = provider->manifest().id;
        if (m_providers.find(id) != m_providers.end()) {
            return core::Result<void>::err(core::Error::make(
                core::ErrorCode::Unknown,
                "Provider already registered with id: " + id
            ));
        }

        m_providers[id] = std::move(provider);
        return core::Result<void>::ok();
    }

    [[nodiscard]] std::shared_ptr<IProvider> find(const std::string& providerId) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_providers.find(providerId);
        if (it != m_providers.end()) {
            return it->second;
        }
        return nullptr;
    }

    [[nodiscard]] std::vector<std::shared_ptr<IProvider>> all() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::shared_ptr<IProvider>> list;
        list.reserve(m_providers.size());
        for (const auto& [_, p] : m_providers) {
            list.push_back(p);
        }
        return list;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_providers.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<IProvider>> m_providers;
};

} // namespace space2x::provider
