#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct SessionAuditMetadata {
    std::uint64_t runtimeCycle = 0;
    std::string safetyAction = "unspecified";
    bool renderAllowed = true;
    bool emergencyStop = false;
};

struct SessionAuditEvent {
    std::uint64_t sequenceId = 0;
    std::int64_t timestampMs = 0;
    std::string category;
    std::string message;
    float riskScore = 0.0f;
    std::string previousHash;
    std::string eventHash;
    std::uint64_t runtimeCycle = 0;
    std::string safetyAction = "unspecified";
    bool renderAllowed = true;
    bool emergencyStop = false;
};

class SessionSafetyJournal {
public:
    SessionAuditEvent record(
        const std::string& category,
        const std::string& message,
        float riskScore);

    SessionAuditEvent record(
        const std::string& category,
        const std::string& message,
        float riskScore,
        const SessionAuditMetadata& metadata);

    const std::vector<SessionAuditEvent>& events() const;
    const SessionAuditEvent* lastEvent() const;
    std::string latestHash() const;
    bool verifyChain() const;
    bool saveToFile(const std::string& path) const;
    bool saveToFile(const std::string& sessionPath, const std::string& latestPath) const;
    bool loadFromFile(const std::string& path);
    void clear();

private:
    std::string computeHash(const SessionAuditEvent& event) const;

private:
    std::vector<SessionAuditEvent> events_;
};

} // namespace dnxr
