#pragma once

#include "neural_chip/DecisionTraceCoverageValidator.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"

#include <string>
#include <vector>

namespace dnxr {

struct PipelineStateTransition {
    NeuralChipPipelineState from;
    NeuralChipPipelineState to;
    std::string reason;
    double timestampMs;
    PipelinePathKind path;
};

class PipelineStateTransitionAudit {
public:
    void record(NeuralChipPipelineState from, NeuralChipPipelineState to, const char* reason, double timestampMs, PipelinePathKind path);
    const std::vector<PipelineStateTransition>& transitions() const;

private:
    std::vector<PipelineStateTransition> transitions_{};
};

} // namespace dnxr
