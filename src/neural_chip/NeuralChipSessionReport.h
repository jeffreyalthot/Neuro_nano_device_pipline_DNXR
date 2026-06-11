#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/NeuralChipSessionState.h"

#include <string>
#include <vector>

namespace dnxr {

struct NeuralChipSessionReport {
    std::uint64_t sessionId = 0;
    std::uint64_t acceptedFrames = 0;
    std::uint64_t rejectedFrames = 0;
    std::uint64_t holdFrames = 0;
    float maxRisk01 = 0.0f;
    float maxFatigue01 = 0.0f;
    std::string mostReducedModality;
    std::uint64_t latchCount = 0;
    std::uint64_t chemicalHoldCount = 0;
    std::uint64_t realityAnchorFailureCount = 0;
    std::vector<std::string> lastDecisionSummaries;
};

class NeuralChipSessionReportBuilder {
public:
    static NeuralChipSessionReport build(const NeuralChipSessionState& state, const NeuralChipReplayRecorder& recorder);
};

} // namespace dnxr
