#include "SessionSafetyJournal.h"

#include "AuditFileWriter.h"
#include "TimeSync.h"

#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {

std::uint64_t fnv1a64(const std::string& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

SessionAuditEvent SessionSafetyJournal::record(
    const std::string& category,
    const std::string& message,
    float riskScore) {
    return record(category, message, riskScore, SessionAuditMetadata{});
}

SessionAuditEvent SessionSafetyJournal::record(
    const std::string& category,
    const std::string& message,
    float riskScore,
    const SessionAuditMetadata& metadata) {

    SessionAuditEvent event;
    event.sequenceId = static_cast<std::uint64_t>(events_.size() + 1U);
    event.timestampMs = TimeSync::nowMs();
    event.category = category;
    event.message = message;
    event.riskScore = riskScore;
    event.previousHash = events_.empty() ? "genesis" : events_.back().eventHash;
    event.runtimeCycle = metadata.runtimeCycle;
    event.safetyAction = metadata.safetyAction;
    event.renderAllowed = metadata.renderAllowed;
    event.emergencyStop = metadata.emergencyStop;
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
    std::uint64_t expectedSequence = 1U;

    for (const auto& event : events_) {
        if (event.sequenceId != expectedSequence) {
            return false;
        }

        if (event.previousHash != previous) {
            return false;
        }

        if (event.eventHash != computeHash(event)) {
            return false;
        }

        previous = event.eventHash;
        ++expectedSequence;
    }

    return true;
}

bool SessionSafetyJournal::saveToFile(const std::string& path) const {
    return AuditFileWriter::writeEvents(path, events_);
}

bool SessionSafetyJournal::saveToFile(const std::string& sessionPath, const std::string& latestPath) const {
    const auto result = AuditFileWriter::writeSessionAndLatest(sessionPath, latestPath, events_);
    return result.sessionWritten && result.latestWritten;
}

bool SessionSafetyJournal::loadFromFile(const std::string& path) {
    std::vector<SessionAuditEvent> loaded;
    if (!AuditFileWriter::readEvents(path, loaded)) {
        return false;
    }

    const auto original = events_;
    events_ = std::move(loaded);
    if (!verifyChain()) {
        events_ = original;
        return false;
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
            << std::setprecision(9) << event.riskScore << '|'
            << event.previousHash << '|'
            << event.runtimeCycle << '|'
            << event.safetyAction << '|'
            << (event.renderAllowed ? 1 : 0) << '|'
            << (event.emergencyStop ? 1 : 0);

    const auto digest = fnv1a64(payload.str());
    std::ostringstream out;
    out << "audit_" << std::hex << digest;
    return out.str();
}

} // namespace dnxr
