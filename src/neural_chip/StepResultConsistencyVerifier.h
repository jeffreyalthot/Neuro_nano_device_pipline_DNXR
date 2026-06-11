#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"

#include <string>

namespace dnxr {

struct StepResultConsistencyReport {
    bool valid = true;
    std::string reason;
};

class StepResultConsistencyVerifier {
public:
    StepResultConsistencyReport verify(const PipelineStepResult& step) const;
    StepResultConsistencyReport verify(const NeuralChipReplayRecord& record) const;
};

} // namespace dnxr
