#pragma once

#include "neural_chip/DecisionTraceCoverageValidator.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"

#include <string>

namespace dnxr {

enum class PipelineStepOutcome {
    NormalAccepted,
    HoldCommitted,
    WaitingForAck,
    Rejected,
    EmergencyLatched,
    TraceInvalid,
    Shutdown,
    NoOp
};

struct PipelineStepResult {
    PipelineStepOutcome outcome = PipelineStepOutcome::Rejected;
    PipelinePathKind path = PipelinePathKind::ValidatorReject;
    NeuralChipPipelineState state = NeuralChipPipelineState::Running;
    bool busSubmitted = false;
    bool replayRecorded = false;
    std::string reason;
};

} // namespace dnxr
