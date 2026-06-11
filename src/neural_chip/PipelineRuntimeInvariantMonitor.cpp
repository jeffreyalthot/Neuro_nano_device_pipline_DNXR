#include "neural_chip/PipelineRuntimeInvariantMonitor.h"

namespace dnxr {

PipelineRuntimeInvariantReport PipelineRuntimeInvariantMonitor::evaluate(const PipelineStepResult& step,
                                                                         const NeuralChipPipelineStateMachine& stateMachine,
                                                                         const NeuralChipAckState& ackState,
                                                                         const NeuralChipEmergencyLatch& latch,
                                                                         const NeuralChipReplayRecord& replay) const {
    if (stateMachine.state() == NeuralChipPipelineState::AwaitingAck && !ackState.hasPendingAck) return {false, "AwaitingAck without pending ACK", true};
    if (stateMachine.state() == NeuralChipPipelineState::Running && ackState.hasPendingAck) return {false, "Running with pending ACK", true};
    if (step.outcome == PipelineStepOutcome::NormalAccepted && !ackState.hasPendingAck) return {false, "NormalAccepted without expected ACK", true};
    if (replay.noOp && replay.busSubmitted) return {false, "no-op with busSubmitted", true};
    if (stateMachine.state() == NeuralChipPipelineState::EmergencyLatched && !latch.latched()) return {false, "EmergencyLatched without active latch", true};
    if (stateMachine.state() == NeuralChipPipelineState::Shutdown && step.busSubmitted) return {false, "Shutdown with bus submit", true};
    return {true, "valid", false};
}

} // namespace dnxr
