#include "neural_chip/ReplayConsistencyVerifier.h"

namespace dnxr {

ReplayConsistencyReport ReplayConsistencyVerifier::verify(const NeuralChipReplayRecorder& recorder) const {
    std::uint64_t lastDigestFrameCount = 0;
    std::size_t count = 0;
    for (const auto& record : recorder.records()) {
        if (record.commitResult.safetyHold && (!record.commitResult.holdFallback || record.commitResult.holdFallback[0] == '\0')) return {false, "safety hold missing fallback", count};
        if (record.commitResult.accepted && record.commitResult.safetyHold) return {false, "accepted commit cannot be safety hold", count};
        if (record.emergencyLatched && record.pipelinePathKind != PipelinePathKind::EmergencyLatchHold && record.pipelineState != NeuralChipPipelineState::ControlledResume) return {false, "emergency latch path mismatch", count};
        if (record.sessionDigestFrameCount < lastDigestFrameCount) return {false, "non-monotone session digest", count};
        if (record.decisionTrace.empty()) return {false, "missing decision trace", count};
        if (record.pipelinePathKind == PipelinePathKind::NormalCommit && record.pipelineState != NeuralChipPipelineState::AwaitingAck && record.pipelineState != NeuralChipPipelineState::Running) return {false, "normal commit state mismatch", count};
        lastDigestFrameCount = record.sessionDigestFrameCount;
        ++count;
    }
    return {count > 0, count > 0 ? "valid" : "empty replay", count};
}

} // namespace dnxr
