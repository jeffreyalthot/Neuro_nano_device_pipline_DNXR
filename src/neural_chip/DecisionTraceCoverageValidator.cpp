#include "neural_chip/DecisionTraceCoverageValidator.h"

#include <algorithm>
#include <string>

namespace dnxr {

const std::vector<const char*>& DecisionTraceCoverageValidator::requiredStages() {
    static const std::vector<const char*> stages = {"PatientModalityPolicyGate","ExperimentalChemicalChannelGate","ClosedLoopPerceptionErrorController","MultisensoryPhaseLockOrchestrator","ModalityExposureLedger","NeuralChipSessionSupervisor","ChipCapabilityContract","PerModalitySafetyEnvelope","ChipRealityAnchorHardChannel","NeuralVectorSlewRateLimiter","NeuralChipFrameCommitter"};
    return stages;
}

DecisionTraceCoverageReport DecisionTraceCoverageValidator::validate(const NeuralChipPipelineDecisionTrace& trace) const {
    for (const char* stage : requiredStages()) {
        const bool found = std::any_of(trace.events().begin(), trace.events().end(), [&](const PipelineDecisionEvent& e){ return std::string(e.stage ? e.stage : "") == stage; });
        if (!found) return {false, stage};
    }
    return {true, ""};
}

} // namespace dnxr
