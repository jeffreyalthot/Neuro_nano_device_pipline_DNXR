#include "neural_chip/PipelineResumeStateBridge.h"

namespace dnxr {

void PipelineResumeStateBridge::synchronizeBeforeDecision(NeuralChipPipelineStateMachine& stateMachine, PipelineResumeController& resumeController, bool emergencyLatched) const {
    if (emergencyLatched) {
        resumeController.lock();
        stateMachine.triggerEmergency("emergency latch active");
        return;
    }
    if (stateMachine.state() == NeuralChipPipelineState::ControlledResume && resumeController.normal()) {
        resumeController.beginControlledResume();
    }
}

void PipelineResumeStateBridge::synchronizeAfterDecision(NeuralChipPipelineStateMachine& stateMachine, const PipelineResumeController& resumeController, const ResumeDecision& decision) const {
    if (stateMachine.state() == NeuralChipPipelineState::ControlledResume && resumeController.normal() && decision.phase == ResumePhase::Normal) {
        stateMachine.onResumeRampComplete();
    }
}

} // namespace dnxr
