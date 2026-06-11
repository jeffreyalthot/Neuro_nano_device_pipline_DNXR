#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/PipelinePathClassifier.h"

#include <fstream>
#include <string>

namespace dnxr {
namespace {
std::string jsonEscape(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        if (c == '"' || c == '\\') out.push_back('\\');
        if (c == '\n' || c == '\r') out.push_back(' ');
        else out.push_back(c);
    }
    return out;
}
const char* toString(PipelineStepOutcome outcome) {
    switch (outcome) {
        case PipelineStepOutcome::NormalAccepted: return "NormalAccepted";
        case PipelineStepOutcome::HoldCommitted: return "HoldCommitted";
        case PipelineStepOutcome::WaitingForAck: return "WaitingForAck";
        case PipelineStepOutcome::Rejected: return "Rejected";
        case PipelineStepOutcome::EmergencyLatched: return "EmergencyLatched";
        case PipelineStepOutcome::TraceInvalid: return "TraceInvalid";
        case PipelineStepOutcome::Shutdown: return "Shutdown";
        case PipelineStepOutcome::NoOp: return "NoOp";
    }
    return "Rejected";
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
        const std::string holdReason = !record.holdReason.empty() ? record.holdReason : record.commitResult.reason;
        out << "{\"schemaVersion\":\"" << DNXR_REPLAY_SCHEMA_VERSION << "\""
            << ",\"frameId\":" << record.frameId
            << ",\"timestampMs\":" << record.timestampMs
            << ",\"pipelineState\":\"" << toString(record.pipelineState) << "\""
            << ",\"pipelinePathKind\":\"" << toString(record.pipelinePathKind) << "\""
            << ",\"stepOutcome\":\"" << toString(record.stepOutcome) << "\""
            << ",\"noOp\":" << (record.noOp ? "true" : "false")
            << ",\"busSubmitted\":" << (record.busSubmitted ? "true" : "false")
            << ",\"candidateAccepted\":" << (record.candidateAccepted ? "true" : "false")
            << ",\"normalFrameSubmitted\":" << (record.normalFrameSubmitted ? "true" : "false")
            << ",\"holdFrameSubmitted\":" << (record.holdFrameSubmitted ? "true" : "false")
            << ",\"safetyHold\":" << (record.safetyHold ? "true" : "false")
            << ",\"lifecycleCommitted\":" << (record.lifecycleCommitted ? "true" : "false")
            << ",\"hasCandidateFrame\":" << (record.hasCandidateFrame ? "true" : "false")
            << ",\"hasSubmittedFrame\":" << (record.hasSubmittedFrame ? "true" : "false")
            << ",\"traceCoverageComplete\":" << (record.traceCoverageComplete ? "true" : "false")
            << ",\"traceCoverageMissingStage\":\"" << jsonEscape(record.traceCoverageMissingStage) << "\""
            << ",\"commitAccepted\":" << ((record.commitResult.candidateAccepted || record.commitResult.accepted) ? "true" : "false")
            << ",\"holdFallback\":\"" << jsonEscape(record.commitResult.holdFallback) << "\""
            << ",\"holdReason\":\"" << jsonEscape(holdReason) << "\""
            << ",\"emergencyLatched\":" << (record.emergencyLatched ? "true" : "false")
            << ",\"sessionRisk01\":" << record.sessionRisk01
            << ",\"sessionFatigue01\":" << record.sessionFatigue01
            << ",\"sessionDigestHash\":" << record.sessionDigestHash
            << ",\"sessionDigestFrameCount\":" << record.sessionDigestFrameCount
            << ",\"decisionTraceCount\":" << record.decisionTrace.size()
            << "}\n";
    }
    return static_cast<bool>(out);
}

} // namespace dnxr
