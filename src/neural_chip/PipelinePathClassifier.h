#pragma once

#include "neural_chip/DecisionTraceCoverageValidator.h"
#include "neural_chip/NeuralChipFrameCommitter.h"

namespace dnxr {

class PipelinePathClassifier {
public:
    PipelinePathKind classifyRejectReason(const char* reason) const;
    PipelinePathKind classifyCommitResult(const FrameCommitResult& result) const;
};

const char* toString(PipelinePathKind path);
PipelinePathKind pipelinePathKindFromString(const char* value);

} // namespace dnxr
