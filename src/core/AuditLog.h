#pragma once

#include <space2x/core/Error.h>
#include <space2x/core/Result.h>
#include <mutex>
#include <string>
#include <vector>

namespace space2x::core {

struct AuditEvent {
    std::string timestamp{};
    std::string action{};
    std::string targetId{};
    std::string outcome{};
    std::string detail{};
};

class AuditLog {
public:
    AuditLog();
    explicit AuditLog(std::string logFilePath);
    ~AuditLog() = default;

    Result<void> record(const std::string& action,
                        const std::string& targetId,
                        const std::string& outcome,
                        const std::string& detail = "");

    [[nodiscard]] std::vector<AuditEvent> getRecentEvents(size_t limit = 100) const;
    [[nodiscard]] const std::string&      logFilePath() const noexcept;

private:
    [[nodiscard]] static std::string currentTimestampIso8601();
    [[nodiscard]] static std::string sanitize(const std::string& input);

    std::string                m_logFilePath;
    mutable std::mutex         m_mutex;
    std::vector<AuditEvent>    m_inMemoryCache;
};

} // namespace space2x::core
