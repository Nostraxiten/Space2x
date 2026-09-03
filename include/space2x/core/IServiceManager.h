#pragma once

#include "Error.h"
#include "Result.h"
#include "ServiceInfo.h"
#include "ServiceState.h"

#include <string>
#include <vector>

namespace space2x::core {

class IServiceManager {
public:
    virtual ~IServiceManager() = default;

    [[nodiscard]] virtual Result<std::vector<ServiceInfo>> listServices() = 0;
    [[nodiscard]] virtual Result<ServiceState>             getState(const std::string& serviceId) = 0;
    [[nodiscard]] virtual Result<void>                     start(const std::string& serviceId) = 0;
    [[nodiscard]] virtual Result<void>                     stop(const std::string& serviceId) = 0;
    [[nodiscard]] virtual Result<void>                     restart(const std::string& serviceId) = 0;
    [[nodiscard]] virtual Result<void>                     setAutoStart(const std::string& serviceId, bool enabled) = 0;
};

} // namespace space2x::core
