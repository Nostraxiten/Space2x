#include "VersionStore.h"

namespace space2x::core {

void VersionStore::registerServiceVersion(const ServiceVersionInfo& info) {
    m_store[info.serviceId] = info;
}

Result<ServiceVersionInfo> VersionStore::getVersionInfo(const std::string& serviceId) const {
    auto it = m_store.find(serviceId);
    if (it != m_store.end()) {
        return Result<ServiceVersionInfo>::ok(it->second);
    }
    return Result<ServiceVersionInfo>::err(
        Error::make(ErrorCode::ServiceNotFound, "Version metadata not found for service: " + serviceId)
    );
}

std::vector<ServiceVersionInfo> VersionStore::all() const {
    std::vector<ServiceVersionInfo> list;
    list.reserve(m_store.size());
    for (const auto& [_, info] : m_store) {
        list.push_back(info);
    }
    return list;
}

} // namespace space2x::core
