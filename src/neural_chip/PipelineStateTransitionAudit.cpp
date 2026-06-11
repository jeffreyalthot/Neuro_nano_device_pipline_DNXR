#include "neural_chip/PipelineStateTransitionAudit.h"

namespace dnxr {

void PipelineStateTransitionAudit::record(NeuralChipPipelineState from, NeuralChipPipelineState to, const char* reason, double timestampMs, PipelinePathKind path) {
    if (from == to) return;
    transitions_.push_back({from, to, reason ? reason : "transition", timestampMs, path});
}

const std::vector<PipelineStateTransition>& PipelineStateTransitionAudit::transitions() const { return transitions_; }

} // namespace dnxr
