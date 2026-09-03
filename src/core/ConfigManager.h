#pragma once

#include <space2x/core/ConfigDelta.h>
#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <string>
#include <vector>

namespace space2x::core {

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    [[nodiscard]] Result<std::string> readFile(const std::string& filePath) const;
    [[nodiscard]] Result<ConfigDelta> computeDelta(const std::string& filePath,
                                                  const std::vector<ConfigDeltaItem>& requestedChanges) const;
    Result<void> applyDelta(const ConfigDelta& delta, bool createBackup = true);
    Result<void> rollback(const std::string& filePath, const std::string& backupPath);

private:
    [[nodiscard]] static std::string generateBackupPath(const std::string& originalPath);
};

} // namespace space2x::core
