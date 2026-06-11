#pragma once

#include "neural_chip/NeuralChipPipelineDecisionTrace.h"

#include <vector>

namespace dnxr {

struct DecisionTraceCoverageReport {
    bool complete = false;
    const char* missingStage = "";
};

class DecisionTraceCoverageValidator {
public:
    DecisionTraceCoverageReport validate(const NeuralChipPipelineDecisionTrace& trace) const;
    static const std::vector<const char*>& requiredStages();
};

} // namespace dnxr
