#pragma once

#include "neural_chip/NeuralChipPipelineStateMachine.h"

namespace dnxr {

class StateMachineTransitionValidator {
public:
    bool allows(NeuralChipPipelineState from, NeuralChipPipelineState to) const;
};

} // namespace dnxr
