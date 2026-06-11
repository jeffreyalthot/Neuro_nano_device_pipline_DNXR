#include "neural_chip/ReplayConsistencyVerifier.h"
#include "neural_chip/StepResultConsistencyVerifier.h"

namespace dnxr {

ReplayConsistencyReport ReplayConsistencyVerifier::verify(const NeuralChipReplayRecorder& recorder) const {
    std::uint64_t lastDigestFrameCount = 0;
    std::size_t count = 0;
    for (const auto& record : recorder.records()) {
        const auto stepReport = StepResultConsistencyVerifier{}.verify(record);
        if (!stepReport.valid) return {false, stepReport.reason.c_str(), count};
        if (!record.noOp && record.commitResult.safetyHold && record.commitResult.holdFallback.empty()) return {false, "safety hold missing fallback", count};
        if ((record.commitResult.candidateAccepted || record.commitResult.accepted) && record.commitResult.safetyHold) return {false, "accepted commit cannot be safety hold", count};
        if (record.emergencyLatched && record.pipelinePathKind != PipelinePathKind::EmergencyLatchHold && record.pipelineState != NeuralChipPipelineState::ControlledResume && record.pipelineState != NeuralChipPipelineState::EmergencyLatched) return {false, "emergency latch path mismatch", count};
        if (record.sessionDigestFrameCount < lastDigestFrameCount) return {false, "non-monotone session digest", count};
        if (record.decisionTrace.empty()) return {false, "missing decision trace", count};
        if (!record.traceCoverageComplete) return {false, "incomplete trace coverage", count};
        if (record.pipelinePathKind == PipelinePathKind::AwaitingAckWait && (!record.noOp || record.commitResult.safetyHold || record.commitResult.candidateAccepted || record.commitResult.accepted)) return {false, "ack wait must be no-op", count};
        if (record.pipelinePathKind == PipelinePathKind::ShutdownNoOp && !record.noOp) return {false, "shutdown path must be no-op", count};
        if (record.pipelinePathKind == PipelinePathKind::NormalCommit && record.pipelineState != NeuralChipPipelineState::AwaitingAck && record.pipelineState != NeuralChipPipelineState::Running) return {false, "normal commit state mismatch", count};
        lastDigestFrameCount = record.sessionDigestFrameCount;
        ++count;
    }
    return {count > 0, count > 0 ? "valid" : "empty replay", count};
}

} // namespace dnxr
