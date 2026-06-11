#pragma once

#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/PipelineResumeController.h"

namespace dnxr {

class PipelineResumeStateBridge {
public:
    void synchronizeBeforeDecision(NeuralChipPipelineStateMachine& stateMachine, PipelineResumeController& resumeController, bool emergencyLatched) const;
    void synchronizeAfterDecision(NeuralChipPipelineStateMachine& stateMachine, const PipelineResumeController& resumeController, const ResumeDecision& decision) const;
};

} // namespace dnxr
