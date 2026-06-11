#include "neural_chip/PipelinePathClassifier.h"

#include <cstring>

namespace dnxr {
namespace {
bool has(const char* text, const char* needle) { return text && needle && std::strstr(text, needle); }
}


PipelinePathKind PipelinePathClassifier::classify(PipelineRejectCode code) const {
    switch (code) {
        case PipelineRejectCode::None: return PipelinePathKind::NormalCommit;
        case PipelineRejectCode::AckMissing: return PipelinePathKind::AckMissingHold;
        case PipelineRejectCode::WorldFrameInvalid: return PipelinePathKind::WorldFrameReject;
        case PipelineRejectCode::EncoderContractFailed: return PipelinePathKind::EarlyEncoderReject;
        case PipelineRejectCode::CapabilityUnsupported: return PipelinePathKind::CapabilityHold;
        case PipelineRejectCode::RealityAnchorFailed: return PipelinePathKind::RealityAnchorHold;
        case PipelineRejectCode::LifecycleRejected: return PipelinePathKind::LifecycleReject;
        case PipelineRejectCode::VectorValidationFailed: return PipelinePathKind::ValidatorReject;
        case PipelineRejectCode::SessionSupervisorHold: return PipelinePathKind::SessionSupervisorHold;
        case PipelineRejectCode::EmergencyLatch: return PipelinePathKind::EmergencyLatchHold;
        case PipelineRejectCode::ReentrantStep: return PipelinePathKind::ReentrantStepReject;
        case PipelineRejectCode::TraceCoverageFailed: return PipelinePathKind::TraceCoverageReject;
        case PipelineRejectCode::Shutdown: return PipelinePathKind::ShutdownNoOp;
    }
    return PipelinePathKind::ValidatorReject;
}

PipelinePathKind PipelinePathClassifier::classifyRejectReason(const char* reason) const {
    if (has(reason, "awaiting") && has(reason, "ack")) return PipelinePathKind::AwaitingAckWait;
    if (has(reason, "shutdown")) return PipelinePathKind::ShutdownNoOp;
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
    if (result.candidateAccepted && !result.safetyHold) return PipelinePathKind::NormalCommit;
    if (has(result.holdFallback.c_str(), "lifecycle") || has(result.reason.c_str(), "lifecycle")) return PipelinePathKind::LifecycleReject;
    return classifyRejectReason(result.reason.c_str());
}

const char* toString(PipelinePathKind path) {
    switch (path) {
        case PipelinePathKind::NormalCommit: return "NormalCommit";
        case PipelinePathKind::AwaitingAckWait: return "AwaitingAckWait";
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
        case PipelinePathKind::ShutdownNoOp: return "ShutdownNoOp";
        case PipelinePathKind::TraceCoverageReject: return "TraceCoverageReject";
        case PipelinePathKind::ReentrantStepReject: return "ReentrantStepReject";
    }
    return "ValidatorReject";
}

PipelinePathKind pipelinePathKindFromString(const char* value) {
    for (PipelinePathKind path : {PipelinePathKind::NormalCommit, PipelinePathKind::AwaitingAckWait, PipelinePathKind::WorldFrameReject, PipelinePathKind::EarlyEncoderReject,
                                 PipelinePathKind::AckMissingHold, PipelinePathKind::EmergencyLatchHold, PipelinePathKind::CapabilityHold,
                                 PipelinePathKind::RealityAnchorHold, PipelinePathKind::LifecycleReject, PipelinePathKind::ValidatorReject,
                                 PipelinePathKind::ResumeHold, PipelinePathKind::SessionSupervisorHold, PipelinePathKind::ShutdownNoOp, PipelinePathKind::TraceCoverageReject, PipelinePathKind::ReentrantStepReject}) {
        if (value && std::strcmp(value, toString(path)) == 0) return path;
    }
    return PipelinePathKind::ValidatorReject;
}

} // namespace dnxr
