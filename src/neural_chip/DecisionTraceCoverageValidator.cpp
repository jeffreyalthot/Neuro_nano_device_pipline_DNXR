#include "neural_chip/DecisionTraceCoverageValidator.h"

#include <algorithm>
#include <string>

namespace dnxr {

const std::vector<const char*>& DecisionTraceCoverageValidator::requiredStages() {
    return requiredStages(PipelinePathKind::NormalCommit);
}

const std::vector<const char*>& DecisionTraceCoverageValidator::requiredStages(PipelinePathKind path) {
    static const std::vector<const char*> normal = {"PatientModalityPolicyGate","ExperimentalChemicalChannelGate","ClosedLoopPerceptionErrorController","MultisensoryPhaseLockOrchestrator","ModalityExposureLedger","NeuralChipSessionSupervisor","ChipCapabilityContract","PerModalitySafetyEnvelope","RealityAnchorDegradationModel","ChipRealityAnchorHardChannel","NeuralVectorSlewRateLimiter","NeuralChipFrameCommitter"};
    static const std::vector<const char*> worldReject = {"WorldFrameSanitizer", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> earlyReject = {"SensorEncoderOutputContract", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> ackWait = {"NeuralChipPipelineStateGate", "NeuralChipAckHandshake"};
    static const std::vector<const char*> ackHold = {"NeuralChipAckHandshake", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> shutdownNoOp = {"NeuralChipPipelineStateGate"};
    static const std::vector<const char*> emergency = {"NeuralChipEmergencyLatch", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> capability = {"ChipCapabilityContract", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> anchor = {"RealityAnchorDegradationModel", "ChipRealityAnchorHardChannel", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> lifecycle = {"FrameLifecycleGuard", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> validator = {"NeuralVectorValidator", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> resume = {"PipelineResumeController", "NeuralChipFrameCommitter"};
    static const std::vector<const char*> supervisor = {"NeuralChipSessionSupervisor", "NeuralChipFrameCommitter"};
    switch (path) {
        case PipelinePathKind::AwaitingAckWait: return ackWait;
        case PipelinePathKind::WorldFrameReject: return worldReject;
        case PipelinePathKind::EarlyEncoderReject: return earlyReject;
        case PipelinePathKind::AckMissingHold: return ackHold;
        case PipelinePathKind::EmergencyLatchHold: return emergency;
        case PipelinePathKind::CapabilityHold: return capability;
        case PipelinePathKind::RealityAnchorHold: return anchor;
        case PipelinePathKind::LifecycleReject: return lifecycle;
        case PipelinePathKind::ValidatorReject: return validator;
        case PipelinePathKind::ResumeHold: return resume;
        case PipelinePathKind::SessionSupervisorHold: return supervisor;
        case PipelinePathKind::ShutdownNoOp: return shutdownNoOp;
        case PipelinePathKind::NormalCommit: return normal;
    }
    return normal;
}

DecisionTraceCoverageReport DecisionTraceCoverageValidator::validate(const NeuralChipPipelineDecisionTrace& trace) const {
    return validate(trace, PipelinePathKind::NormalCommit);
}

DecisionTraceCoverageReport DecisionTraceCoverageValidator::validate(const NeuralChipPipelineDecisionTrace& trace, PipelinePathKind path) const {
    for (const char* stage : requiredStages(path)) {
        const bool found = std::any_of(trace.events().begin(), trace.events().end(), [&](const PipelineDecisionEvent& e){ return std::string(e.stage ? e.stage : "") == stage; });
        if (!found) return {false, false, stage};
    }
    return {true, true, ""};
}

} // namespace dnxr
