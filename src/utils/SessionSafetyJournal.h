#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct SessionAuditEvent {
    std::uint64_t sequenceId = 0;
    std::int64_t timestampMs = 0;
    std::string category;
    std::string message;
    float riskScore = 0.0f;
    std::string previousHash;
    std::string eventHash;
};

class SessionSafetyJournal {
public:
    SessionAuditEvent record(
        const std::string& category,
        const std::string& message,
        float riskScore);

    const std::vector<SessionAuditEvent>& events() const;
    const SessionAuditEvent* lastEvent() const;
    std::string latestHash() const;
    bool verifyChain() const;
    void clear();

private:
    std::string computeHash(const SessionAuditEvent& event) const;

private:
    std::vector<SessionAuditEvent> events_;
};

} // namespace dnxr
