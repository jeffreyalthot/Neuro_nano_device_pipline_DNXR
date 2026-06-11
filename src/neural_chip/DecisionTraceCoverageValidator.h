#pragma once

#include "neural_chip/NeuralChipPipelineDecisionTrace.h"

#include <vector>

namespace dnxr {

enum class PipelinePathKind {
    NormalCommit,
    AwaitingAckWait,
    AckMissingHold,
    WorldFrameReject,
    EarlyEncoderReject,
    EmergencyLatchHold,
    CapabilityHold,
    RealityAnchorHold,
    LifecycleReject,
    ValidatorReject,
    ResumeHold,
    SessionSupervisorHold,
    ShutdownNoOp
};

struct DecisionTraceCoverageReport {
    bool complete = false;
    bool accepted = false;
    const char* missingStage = "";
};

class DecisionTraceCoverageValidator {
public:
    DecisionTraceCoverageReport validate(const NeuralChipPipelineDecisionTrace& trace) const;
    DecisionTraceCoverageReport validate(const NeuralChipPipelineDecisionTrace& trace, PipelinePathKind path) const;
    static const std::vector<const char*>& requiredStages();
    static const std::vector<const char*>& requiredStages(PipelinePathKind path);
};

} // namespace dnxr
