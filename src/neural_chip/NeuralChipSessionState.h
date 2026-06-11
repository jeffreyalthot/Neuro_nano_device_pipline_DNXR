#pragma once

#include <cstdint>

namespace dnxr {

struct NeuralChipSessionState {
    std::uint64_t sessionId = 0;
    std::uint64_t acceptedFrames = 0;
    std::uint64_t rejectedFrames = 0;
    std::uint64_t holdFrames = 0;
    float sessionRisk01 = 0.0f;
    float sessionFatigue01 = 0.0f;
    float realityAnchorStability01 = 1.0f;
    float motorConfidenceRolling01 = 0.0f;
    float closedLoopStability01 = 1.0f;
    bool chemicalChannelsEverEnabled = false;
    bool emergencyStopLatched = false;
};

void resetNeuralChipSessionState(NeuralChipSessionState& state, std::uint64_t sessionId = 0);

} // namespace dnxr
