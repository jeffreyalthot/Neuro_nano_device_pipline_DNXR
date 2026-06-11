#include "neural_chip/StepResultConsistencyVerifier.h"

namespace dnxr {

StepResultConsistencyReport StepResultConsistencyVerifier::verify(const PipelineStepResult& step) const {
    if (step.outcome == PipelineStepOutcome::NormalAccepted && (!step.busSubmitted || step.path != PipelinePathKind::NormalCommit)) return {false, "normal accepted must submit normal bus commit"};
    if (step.outcome == PipelineStepOutcome::WaitingForAck && step.busSubmitted) return {false, "waiting for ack must be no-op without bus submit"};
    if (step.outcome == PipelineStepOutcome::Shutdown && step.busSubmitted) return {false, "shutdown must not submit to bus"};
    if (step.outcome == PipelineStepOutcome::TraceInvalid && step.path == PipelinePathKind::NormalCommit) return {false, "trace invalid cannot be normal commit"};
    if (step.state == NeuralChipPipelineState::EmergencyLatched && step.path != PipelinePathKind::EmergencyLatchHold && step.outcome != PipelineStepOutcome::EmergencyLatched) return {false, "emergency latched state must use emergency path or outcome"};
    return {true, "valid"};
}

StepResultConsistencyReport StepResultConsistencyVerifier::verify(const NeuralChipReplayRecord& record) const {
    PipelineStepResult step{record.stepOutcome, record.pipelinePathKind, record.pipelineState, record.busSubmitted, true, record.holdReason};
    const auto base = verify(step);
    if (!base.valid) return base;
    if (record.stepOutcome == PipelineStepOutcome::NormalAccepted && (!record.busSubmitted || !record.normalFrameSubmitted || !record.hasCandidateFrame || !record.hasSubmittedFrame)) return {false, "normal accepted replay missing submitted frame flags"};
    if (record.stepOutcome == PipelineStepOutcome::WaitingForAck && (!record.noOp || record.busSubmitted || record.hasCandidateFrame || record.hasSubmittedFrame)) return {false, "waiting replay must be no-op without frames"};
    if (record.stepOutcome == PipelineStepOutcome::HoldCommitted && (!record.holdFrameSubmitted && !record.safetyHold)) return {false, "hold committed replay missing hold submission"};
    if (record.noOp && record.busSubmitted) return {false, "no-op replay cannot submit bus"};
    return {true, "valid"};
}

} // namespace dnxr
