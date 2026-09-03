#include "AuditLog.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

namespace space2x::core {

AuditLog::AuditLog()
    : AuditLog("space2x-audit.log") {}

AuditLog::AuditLog(std::string logFilePath)
    : m_logFilePath(std::move(logFilePath)) {}

std::string AuditLog::currentTimestampIso8601() {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tmBuffer{};
#if defined(_WIN32)
    gmtime_s(&tmBuffer, &timeT);
#else
    gmtime_r(&timeT, &tmBuffer);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmBuffer, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count()
        << 'Z';
    return oss.str();
}

std::string AuditLog::sanitize(const std::string& input) {
    static const std::regex passwordRegex(
        R"((password|passwd|secret|token|auth)\s*[:=]\s*([^\s,;]+))",
        std::regex_constants::icase
    );
    return std::regex_replace(input, passwordRegex, "$1=********");
}

void AuditLog::record(const std::string& action,
                      const std::string& targetId,
                      const std::string& outcome,
                      const std::string& detail) noexcept {
    try {
        AuditEvent event;
        event.timestamp = currentTimestampIso8601();
        event.action    = sanitize(action);
        event.targetId  = sanitize(targetId);
        event.outcome   = sanitize(outcome);
        event.detail    = sanitize(detail);

        std::lock_guard<std::mutex> lock(m_mutex);
        m_inMemoryCache.push_back(event);

        // Persist to log file (best-effort — failure is silently swallowed)
        std::ofstream out(m_logFilePath, std::ios::app);
        if (out.is_open()) {
            out << "[" << event.timestamp << "] "
                << "ACTION=" << event.action << " | "
                << "TARGET=" << event.targetId << " | "
                << "OUTCOME=" << event.outcome;
            if (!event.detail.empty()) {
                out << " | DETAIL=" << event.detail;
            }
            out << "\n";
        }
    } catch (...) {
        // Audit log failures are intentionally suppressed.
        // The application must never crash or propagate errors
        // due to an audit write failure.
    }
}

std::vector<AuditEvent> AuditLog::getRecentEvents(size_t limit) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_inMemoryCache.empty()) {
        return {};
    }

    size_t count = std::min(limit, m_inMemoryCache.size());
    std::vector<AuditEvent> result;
    result.reserve(count);

    auto startIt = m_inMemoryCache.end() - static_cast<ptrdiff_t>(count);
    for (auto it = startIt; it != m_inMemoryCache.end(); ++it) {
        result.push_back(*it);
    }

    return result;
}

const std::string& AuditLog::logFilePath() const noexcept {
    return m_logFilePath;
}

} // namespace space2x::core
