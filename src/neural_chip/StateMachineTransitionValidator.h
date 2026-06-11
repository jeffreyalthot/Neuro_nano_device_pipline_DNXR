#pragma once

#include "neural_chip/NeuralChipPipelineStateMachine.h"

#include <string>

namespace dnxr {

struct StateTransitionRequest {
    NeuralChipPipelineState from = NeuralChipPipelineState::ColdStart;
    NeuralChipPipelineState to = NeuralChipPipelineState::ColdStart;
    std::string reason;
    bool auditedReset = false;
    bool operatorAuthorized = false;
};

class StateMachineTransitionValidator {
public:
    bool allows(const StateTransitionRequest& request) const;
    bool allows(NeuralChipPipelineState from, NeuralChipPipelineState to) const;
};

} // namespace dnxr
