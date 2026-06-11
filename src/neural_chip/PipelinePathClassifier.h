#pragma once

#include "neural_chip/DecisionTraceCoverageValidator.h"
#include "neural_chip/NeuralChipFrameCommitter.h"

namespace dnxr {

enum class PipelineRejectCode {
    None, AckMissing, WorldFrameInvalid, EncoderContractFailed, CapabilityUnsupported, RealityAnchorFailed, LifecycleRejected, VectorValidationFailed, SessionSupervisorHold, EmergencyLatch, ReentrantStep, TraceCoverageFailed, Shutdown
};

class PipelinePathClassifier {
public:
    PipelinePathKind classify(PipelineRejectCode code) const;
    PipelinePathKind classifyRejectReason(const char* reason) const;
    PipelinePathKind classifyCommitResult(const FrameCommitResult& result) const;
};

const char* toString(PipelinePathKind path);
PipelinePathKind pipelinePathKindFromString(const char* value);

} // namespace dnxr
