#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/StateMachineTransitionValidator.h"

namespace dnxr {

const char* toString(NeuralChipPipelineState state) {
    switch (state) {
        case NeuralChipPipelineState::ColdStart: return "ColdStart";
        case NeuralChipPipelineState::Running: return "Running";
        case NeuralChipPipelineState::AwaitingAck: return "AwaitingAck";
        case NeuralChipPipelineState::HoldDecompression: return "HoldDecompression";
        case NeuralChipPipelineState::HoldNeutral: return "HoldNeutral";
        case NeuralChipPipelineState::HoldMinimal: return "HoldMinimal";
        case NeuralChipPipelineState::ControlledResume: return "ControlledResume";
        case NeuralChipPipelineState::EmergencyLatched: return "EmergencyLatched";
        case NeuralChipPipelineState::Shutdown: return "Shutdown";
    }
    return "Unknown";
}

NeuralChipPipelineState NeuralChipPipelineStateMachine::state() const { return state_; }
const char* NeuralChipPipelineStateMachine::reason() const { return reason_; }

bool NeuralChipPipelineStateMachine::transition(NeuralChipPipelineState next, const char* reason) {
    if (!StateMachineTransitionValidator{}.allows(state_, next)) return false;
    state_ = next;
    reason_ = reason ? reason : "transition";
    return true;
}

bool NeuralChipPipelineStateMachine::start() {
    if (state_ != NeuralChipPipelineState::ColdStart) return false;
    return transition(NeuralChipPipelineState::Running, "pipeline started");
}

bool NeuralChipPipelineStateMachine::onCommitAccepted() {
    if (state_ != NeuralChipPipelineState::Running && state_ != NeuralChipPipelineState::ControlledResume) return false;
    return transition(NeuralChipPipelineState::AwaitingAck, "commit accepted; awaiting ack");
}

bool NeuralChipPipelineStateMachine::onAckValid() {
    if (state_ != NeuralChipPipelineState::AwaitingAck) return false;
    return transition(NeuralChipPipelineState::Running, "ack accepted");
}

bool NeuralChipPipelineStateMachine::onAckMissingHold() {
    if (state_ == NeuralChipPipelineState::AwaitingAck) return transition(NeuralChipPipelineState::HoldDecompression, "ack missing; decompression hold");
    return false;
}

bool NeuralChipPipelineStateMachine::onPersistentHold() {
    if (state_ == NeuralChipPipelineState::HoldDecompression) return transition(NeuralChipPipelineState::HoldNeutral, "persistent hold; neutral hold");
    if (state_ == NeuralChipPipelineState::HoldNeutral) return transition(NeuralChipPipelineState::HoldMinimal, "persistent hold; minimal hold");
    return false;
}

bool NeuralChipPipelineStateMachine::onStableHold() {
    if (state_ != NeuralChipPipelineState::HoldMinimal && state_ != NeuralChipPipelineState::EmergencyLatched) return false;
    return transition(NeuralChipPipelineState::ControlledResume, "stable hold; controlled resume");
}

bool NeuralChipPipelineStateMachine::onResumeRampComplete() {
    if (state_ != NeuralChipPipelineState::ControlledResume) return false;
    return transition(NeuralChipPipelineState::Running, "controlled resume complete");
}

bool NeuralChipPipelineStateMachine::triggerEmergency(const char* reason) {
    if (state_ == NeuralChipPipelineState::Shutdown) return false;
    return transition(NeuralChipPipelineState::EmergencyLatched, reason ? reason : "critical violation");
}

bool NeuralChipPipelineStateMachine::onAuditedReset() {
    if (state_ != NeuralChipPipelineState::EmergencyLatched) return false;
    return transition(NeuralChipPipelineState::ControlledResume, "audited reset accepted");
}

bool NeuralChipPipelineStateMachine::shutdown(const char* reason) { return transition(NeuralChipPipelineState::Shutdown, reason); }

bool NeuralChipPipelineStateMachine::allowsNormalFrame() const {
    return state_ == NeuralChipPipelineState::Running || state_ == NeuralChipPipelineState::ControlledResume || state_ == NeuralChipPipelineState::ColdStart;
}

bool NeuralChipPipelineStateMachine::awaitingAck() const { return state_ == NeuralChipPipelineState::AwaitingAck; }
bool NeuralChipPipelineStateMachine::inHold() const { return state_ == NeuralChipPipelineState::HoldDecompression || state_ == NeuralChipPipelineState::HoldNeutral || state_ == NeuralChipPipelineState::HoldMinimal; }
bool NeuralChipPipelineStateMachine::emergencyLatched() const { return state_ == NeuralChipPipelineState::EmergencyLatched; }

} // namespace dnxr
