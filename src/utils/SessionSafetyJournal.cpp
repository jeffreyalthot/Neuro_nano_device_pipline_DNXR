#include "SessionSafetyJournal.h"

#include "TimeSync.h"

#include <functional>
#include <sstream>

namespace dnxr {

SessionAuditEvent SessionSafetyJournal::record(
    const std::string& category,
    const std::string& message,
    float riskScore) {

    SessionAuditEvent event;
    event.sequenceId = static_cast<std::uint64_t>(events_.size() + 1U);
    event.timestampMs = TimeSync::nowMs();
    event.category = category;
    event.message = message;
    event.riskScore = riskScore;
    event.previousHash = events_.empty() ? "genesis" : events_.back().eventHash;
    event.eventHash = computeHash(event);

    events_.push_back(event);
    return event;
}

const std::vector<SessionAuditEvent>& SessionSafetyJournal::events() const {
    return events_;
}

const SessionAuditEvent* SessionSafetyJournal::lastEvent() const {
    return events_.empty() ? nullptr : &events_.back();
}

std::string SessionSafetyJournal::latestHash() const {
    return events_.empty() ? "genesis" : events_.back().eventHash;
}

bool SessionSafetyJournal::verifyChain() const {
    std::string previous = "genesis";

    for (const auto& event : events_) {
        if (event.previousHash != previous) {
            return false;
        }

        if (event.eventHash != computeHash(event)) {
            return false;
        }

        previous = event.eventHash;
    }

    return true;
}

void SessionSafetyJournal::clear() {
    events_.clear();
}

std::string SessionSafetyJournal::computeHash(const SessionAuditEvent& event) const {
    std::ostringstream payload;
    payload << event.sequenceId << '|'
            << event.timestampMs << '|'
            << event.category << '|'
            << event.message << '|'
            << event.riskScore << '|'
            << event.previousHash;

    const auto digest = std::hash<std::string>{}(payload.str());
    std::ostringstream out;
    out << "audit_" << std::hex << digest;
    return out.str();
}

} // namespace dnxr
