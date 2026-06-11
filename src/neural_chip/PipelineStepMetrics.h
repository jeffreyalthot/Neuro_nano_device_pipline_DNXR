#pragma once

#include "neural_chip/PipelineStepResult.h"

#include <cstdint>

namespace dnxr {

struct PipelineStepMetrics {
    std::uint64_t totalSteps = 0;
    std::uint64_t normalAccepted = 0;
    std::uint64_t waitingForAck = 0;
    std::uint64_t holdCommitted = 0;
    std::uint64_t rejected = 0;
    std::uint64_t emergencyLatched = 0;
    std::uint64_t shutdown = 0;

    float acceptanceRatio01 = 0.0f;
    float holdRatio01 = 0.0f;
    float waitingRatio01 = 0.0f;
};

void updatePipelineStepMetrics(PipelineStepMetrics& metrics, PipelineStepOutcome outcome);

} // namespace dnxr
