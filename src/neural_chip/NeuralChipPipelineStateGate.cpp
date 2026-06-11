#include "neural_chip/NeuralChipPipelineStateGate.h"

namespace dnxr {

PipelineStateDecision NeuralChipPipelineStateGate::evaluate(const NeuralChipPipelineStateMachine& stateMachine,
                                                            const NeuralChipAckHandshake& ackHandshake,
                                                            const PipelineResumeController& resumeController,
                                                            double nowMs) const {
    switch (stateMachine.state()) {
        case NeuralChipPipelineState::ColdStart:
        case NeuralChipPipelineState::Running:
            return {true, true, false, PipelineHoldKind::None, 1.0f, "running"};
        case NeuralChipPipelineState::AwaitingAck:
            if (ackHandshake.shouldHold(nowMs)) {
                return {false, false, true, PipelineHoldKind::Decompression, 0.20f, "neural chip ack missing"};
            }
            return {false, false, true, PipelineHoldKind::AwaitingAck, 0.20f, "awaiting neural chip ack"};
        case NeuralChipPipelineState::HoldDecompression:
            return {false, false, true, PipelineHoldKind::Decompression, 0.20f, stateMachine.reason()};
        case NeuralChipPipelineState::HoldNeutral:
            return {false, false, true, PipelineHoldKind::Neutral, 0.0f, stateMachine.reason()};
        case NeuralChipPipelineState::HoldMinimal:
            return {false, false, true, PipelineHoldKind::Minimal, 0.0f, stateMachine.reason()};
        case NeuralChipPipelineState::ControlledResume:
            return {true, true, false, PipelineHoldKind::None, 0.25f, "controlled resume"};
        case NeuralChipPipelineState::EmergencyLatched:
            return {false, false, true, PipelineHoldKind::Emergency, 0.0f, stateMachine.reason()};
        case NeuralChipPipelineState::Shutdown:
            return {false, false, true, PipelineHoldKind::Shutdown, 0.0f, "pipeline shutdown"};
    }
    (void)resumeController;
    return {false, false, true, PipelineHoldKind::Emergency, 0.0f, "invalid pipeline state"};
}

} // namespace dnxr
