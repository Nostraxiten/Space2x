#include <space2x/core/ConfigManager.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace space2x::core {

namespace fs = std::filesystem;

std::string ConfigManager::generateBackupPath(const std::string& originalPath) {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuffer{};
#if defined(_WIN32)
    localtime_s(&tmBuffer, &timeT);
#else
    localtime_r(&timeT, &tmBuffer);
#endif

    std::ostringstream oss;
    oss << originalPath << ".bak."
        << std::put_time(&tmBuffer, "%Y%m%d%H%M%S");
    return oss.str();
}

Result<std::string> ConfigManager::readFile(const std::string& filePath) const {
    if (!fs::exists(filePath)) {
        return Result<std::string>::err(Error::make(
            ErrorCode::ConfigFileNotFound,
            "Configuration file does not exist: " + filePath,
            "",
            "Verify that the service is installed and the configuration path is valid."
        ));
    }

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return Result<std::string>::err(Error::make(
            ErrorCode::PermissionDenied,
            "Cannot open configuration file for reading: " + filePath,
            "",
            "Check file permissions or run with elevated privileges."
        ));
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return Result<std::string>::ok(ss.str());
}

Result<ConfigDelta> ConfigManager::computeDelta(const std::string& filePath,
                                               const std::vector<ConfigDeltaItem>& requestedChanges) const {
    auto contentRes = readFile(filePath);
    if (!contentRes.isOk()) {
        return Result<ConfigDelta>::err(contentRes.error());
    }

    ConfigDelta delta;
    delta.targetFile = filePath;
    delta.changes = requestedChanges;

    std::ostringstream preview;
    preview << "=== Configuration Change Preview: " << filePath << " ===\n";
    for (const auto& item : requestedChanges) {
        preview << "  " << item.key << ": " << item.oldValue << " -> " << item.newValue << "\n";
    }
    delta.rawPreview = preview.str();

    return Result<ConfigDelta>::ok(std::move(delta));
}

Result<void> ConfigManager::applyDelta(const ConfigDelta& delta, bool createBackup) {
    if (!delta.hasChanges()) {
        return Result<void>::ok();
    }

    if (!fs::exists(delta.targetFile)) {
        return Result<void>::err(Error::make(
            ErrorCode::ConfigFileNotFound,
            "Target configuration file does not exist: " + delta.targetFile
        ));
    }

    if (createBackup) {
        std::string backupPath = generateBackupPath(delta.targetFile);
        std::error_code ec;
        fs::copy_file(delta.targetFile, backupPath, fs::copy_options::overwrite_existing, ec);
        if (ec) {
            return Result<void>::err(Error::make(
                ErrorCode::IoError,
                "Failed to create pre-modification backup at: " + backupPath,
                ec.message(),
                "Check disk write permissions."
            ));
        }
    }

    auto contentRes = readFile(delta.targetFile);
    if (!contentRes.isOk()) {
        return Result<void>::err(contentRes.error());
    }

    std::string content = contentRes.value();
    for (const auto& item : delta.changes) {
        // Line-by-line replacement or key search
        size_t pos = content.find(item.oldValue);
        if (pos != std::string::npos) {
            content.replace(pos, item.oldValue.length(), item.newValue);
        }
    }

    std::ofstream outFile(delta.targetFile, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!outFile.is_open()) {
        return Result<void>::err(Error::make(
            ErrorCode::PermissionDenied,
            "Failed to open configuration file for writing: " + delta.targetFile
        ));
    }

    outFile << content;
    outFile.close();

    return Result<void>::ok();
}

Result<void> ConfigManager::rollback(const std::string& filePath, const std::string& backupPath) {
    if (!fs::exists(backupPath)) {
        return Result<void>::err(Error::make(
            ErrorCode::ConfigFileNotFound,
            "Backup file does not exist: " + backupPath
        ));
    }

    std::error_code ec;
    fs::copy_file(backupPath, filePath, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        return Result<void>::err(Error::make(
            ErrorCode::IoError,
            "Failed to restore configuration from backup: " + backupPath,
            ec.message()
        ));
    }

    return Result<void>::ok();
}

} // namespace space2x::core
