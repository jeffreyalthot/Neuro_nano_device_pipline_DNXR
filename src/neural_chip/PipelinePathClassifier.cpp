#include "neural_chip/PipelinePathClassifier.h"

#include <cstring>

namespace dnxr {
namespace {
bool has(const char* text, const char* needle) { return text && needle && std::strstr(text, needle); }
}

PipelinePathKind PipelinePathClassifier::classifyRejectReason(const char* reason) const {
    if (has(reason, "ack")) return PipelinePathKind::AckMissingHold;
    if (has(reason, "world") || has(reason, "timestamp")) return PipelinePathKind::WorldFrameReject;
    if (has(reason, "encoder")) return PipelinePathKind::EarlyEncoderReject;
    if (has(reason, "capability") || has(reason, "disabled")) return PipelinePathKind::CapabilityHold;
    if (has(reason, "reality anchor")) return PipelinePathKind::RealityAnchorHold;
    if (has(reason, "Lifecycle") || has(reason, "lifecycle")) return PipelinePathKind::LifecycleReject;
    if (has(reason, "outside [0,1]") || has(reason, "non-finite") || has(reason, "NeuralVectorValidator")) return PipelinePathKind::ValidatorReject;
    if (has(reason, "closed-loop") || has(reason, "session")) return PipelinePathKind::SessionSupervisorHold;
    if (has(reason, "resume")) return PipelinePathKind::ResumeHold;
    if (has(reason, "emergency") || has(reason, "patient stop") || has(reason, "cutoff")) return PipelinePathKind::EmergencyLatchHold;
    return PipelinePathKind::ValidatorReject;
}

PipelinePathKind PipelinePathClassifier::classifyCommitResult(const FrameCommitResult& result) const {
    if (result.accepted && !result.safetyHold) return PipelinePathKind::NormalCommit;
    if (has(result.holdFallback, "lifecycle") || has(result.reason, "lifecycle")) return PipelinePathKind::LifecycleReject;
    return classifyRejectReason(result.reason);
}

const char* toString(PipelinePathKind path) {
    switch (path) {
        case PipelinePathKind::NormalCommit: return "NormalCommit";
        case PipelinePathKind::WorldFrameReject: return "WorldFrameReject";
        case PipelinePathKind::EarlyEncoderReject: return "EarlyEncoderReject";
        case PipelinePathKind::AckMissingHold: return "AckMissingHold";
        case PipelinePathKind::EmergencyLatchHold: return "EmergencyLatchHold";
        case PipelinePathKind::CapabilityHold: return "CapabilityHold";
        case PipelinePathKind::RealityAnchorHold: return "RealityAnchorHold";
        case PipelinePathKind::LifecycleReject: return "LifecycleReject";
        case PipelinePathKind::ValidatorReject: return "ValidatorReject";
        case PipelinePathKind::ResumeHold: return "ResumeHold";
        case PipelinePathKind::SessionSupervisorHold: return "SessionSupervisorHold";
    }
    return "ValidatorReject";
}

PipelinePathKind pipelinePathKindFromString(const char* value) {
    for (PipelinePathKind path : {PipelinePathKind::NormalCommit, PipelinePathKind::WorldFrameReject, PipelinePathKind::EarlyEncoderReject,
                                 PipelinePathKind::AckMissingHold, PipelinePathKind::EmergencyLatchHold, PipelinePathKind::CapabilityHold,
                                 PipelinePathKind::RealityAnchorHold, PipelinePathKind::LifecycleReject, PipelinePathKind::ValidatorReject,
                                 PipelinePathKind::ResumeHold, PipelinePathKind::SessionSupervisorHold}) {
        if (value && std::strcmp(value, toString(path)) == 0) return path;
    }
    return PipelinePathKind::ValidatorReject;
}

} // namespace dnxr
