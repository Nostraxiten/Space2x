#pragma once

#include "ServiceState.h"
#include <cstdint>
#include <string>

namespace space2x::core {

struct ServiceInfo {
    std::string   id{};
    std::string   displayName{};
    std::string   description{};
    std::string   category{};
    ServiceState  state{ServiceState::Unknown};
    bool          autoStartEnabled{false};
    uint32_t      pid{0};
    uint16_t      port{0};
    std::string   installedVersion{};
    std::string   configPath{};
    std::string   logPath{};
    uint64_t      memoryBytes{0};
    double        cpuPercent{0.0};
};

} // namespace space2x::core
