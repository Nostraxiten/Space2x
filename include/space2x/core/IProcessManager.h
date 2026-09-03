#pragma once

#include "Error.h"
#include "Result.h"

#include <cstdint>
#include <string>
#include <vector>

namespace space2x::core {

struct ProcessInfo {
    uint32_t    pid{0};
    uint32_t    parentPid{0};
    std::string name{};
    std::string executablePath{};
    uint64_t    memoryBytes{0};
    double      cpuPercent{0.0};
};

class IProcessManager {
public:
    virtual ~IProcessManager() = default;

    [[nodiscard]] virtual Result<std::vector<ProcessInfo>> listProcesses() = 0;
    [[nodiscard]] virtual Result<ProcessInfo>              getProcessInfo(uint32_t pid) = 0;
    [[nodiscard]] virtual Result<void>                     killProcess(uint32_t pid) = 0;
};

} // namespace space2x::core
