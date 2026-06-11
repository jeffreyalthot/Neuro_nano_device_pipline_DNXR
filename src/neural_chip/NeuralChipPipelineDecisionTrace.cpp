#include "neural_chip/NeuralChipPipelineDecisionTrace.h"

namespace dnxr {

void NeuralChipPipelineDecisionTrace::record(const PipelineDecisionEvent& event) { events_.push_back(event); }
const std::vector<PipelineDecisionEvent>& NeuralChipPipelineDecisionTrace::events() const { return events_; }
void NeuralChipPipelineDecisionTrace::clear() { events_.clear(); }

} // namespace dnxr
