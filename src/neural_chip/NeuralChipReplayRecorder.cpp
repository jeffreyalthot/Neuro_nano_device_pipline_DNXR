#include "neural_chip/NeuralChipReplayRecorder.h"

#include <fstream>
#include <string>
#include <algorithm>

namespace dnxr {
namespace {
std::string jsonEscape(const char* raw) {
    std::string out;
    const char* s = raw ? raw : "";
    for (; *s; ++s) {
        if (*s == '"' || *s == '\\') out.push_back('\\');
        if (*s == '\n' || *s == '\r') out.push_back(' ');
        else out.push_back(*s);
    }
    return out;
}
} // namespace

NeuralChipReplayRecorder::NeuralChipReplayRecorder(ReplayRecorderPolicy policy) : policy_(policy) {}
void NeuralChipReplayRecorder::setPolicy(ReplayRecorderPolicy policy) { policy_ = policy; }
const ReplayRecorderPolicy& NeuralChipReplayRecorder::policy() const { return policy_; }
void NeuralChipReplayRecorder::record(const NeuralChipReplayRecord& record) {
    if (policy_.maxInMemoryRecords == 0) return;
    if (records_.size() >= policy_.maxInMemoryRecords) {
        if (!policy_.dropOldest) return;
        records_.erase(records_.begin());
    }
    records_.push_back(record);
}
const std::vector<NeuralChipReplayRecord>& NeuralChipReplayRecorder::records() const { return records_; }
void NeuralChipReplayRecorder::clear() { records_.clear(); }

bool NeuralChipReplayRecorder::exportJsonl(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    for (const auto& record : records_) {
        out << "{\"frameId\":" << record.frameId
            << ",\"timestampMs\":" << record.timestampMs
            << ",\"commitAccepted\":" << (record.commitResult.accepted ? "true" : "false")
            << ",\"safetyHold\":" << (record.commitResult.safetyHold ? "true" : "false")
            << ",\"holdFallback\":\"" << jsonEscape(record.commitResult.holdFallback) << "\""
            << ",\"holdReason\":\"" << jsonEscape(record.holdReason ? record.holdReason : record.commitResult.reason) << "\""
            << ",\"emergencyLatched\":" << (record.emergencyLatched ? "true" : "false")
            << ",\"sessionRisk01\":" << record.sessionRisk01
            << ",\"sessionFatigue01\":" << record.sessionFatigue01
            << ",\"decisionTraceCount\":" << record.decisionTrace.size()
            << "}\n";
    }
    return static_cast<bool>(out);
}

} // namespace dnxr
