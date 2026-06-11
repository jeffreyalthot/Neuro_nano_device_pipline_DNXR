#include "neural_chip/StateMachineTransitionValidator.h"

namespace dnxr {

bool StateMachineTransitionValidator::allows(NeuralChipPipelineState from, NeuralChipPipelineState to) const {
    if (from == to) return from == NeuralChipPipelineState::Shutdown;
    switch (from) {
        case NeuralChipPipelineState::ColdStart: return to == NeuralChipPipelineState::Running;
        case NeuralChipPipelineState::Running: return to == NeuralChipPipelineState::AwaitingAck || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::AwaitingAck: return to == NeuralChipPipelineState::Running || to == NeuralChipPipelineState::HoldDecompression || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldDecompression: return to == NeuralChipPipelineState::HoldNeutral || to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldNeutral: return to == NeuralChipPipelineState::HoldMinimal || to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::HoldMinimal: return to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::ControlledResume: return to == NeuralChipPipelineState::Running || to == NeuralChipPipelineState::EmergencyLatched || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::EmergencyLatched: return to == NeuralChipPipelineState::ControlledResume || to == NeuralChipPipelineState::Shutdown;
        case NeuralChipPipelineState::Shutdown: return to == NeuralChipPipelineState::Shutdown;
    }
    return false;
}

} // namespace dnxr
