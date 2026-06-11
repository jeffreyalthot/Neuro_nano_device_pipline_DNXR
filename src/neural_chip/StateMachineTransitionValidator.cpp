#include "neural_chip/StateMachineTransitionValidator.h"

namespace dnxr {

bool StateMachineTransitionValidator::allows(NeuralChipPipelineState from, NeuralChipPipelineState to) const {
    return allows({from, to, "legacy transition", false, false});
}

bool StateMachineTransitionValidator::allows(const StateTransitionRequest& request) const {
    const auto from = request.from;
    const auto to = request.to;
    if (from == to) return from == NeuralChipPipelineState::Shutdown;
    switch (from) {
        case NeuralChipPipelineState::ColdStart: return to == NeuralChipPipelineState::Running;
        case NeuralChipPipelineState::Running: return to == NeuralChipPipelineState::AwaitingAck || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::AwaitingAck: return to == NeuralChipPipelineState::Running || to == NeuralChipPipelineState::HoldDecompression || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldDecompression: return to == NeuralChipPipelineState::HoldNeutral || to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldNeutral: return to == NeuralChipPipelineState::HoldMinimal || to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldMinimal: return to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::ControlledResume: return to == NeuralChipPipelineState::Running || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::EmergencyLatched:
            if (to == NeuralChipPipelineState::Shutdown) return true;
            return to == NeuralChipPipelineState::ControlledResume && request.auditedReset && request.operatorAuthorized;
        case NeuralChipPipelineState::Shutdown: return to == NeuralChipPipelineState::Shutdown;
    }
    return false;
}

} // namespace dnxr
