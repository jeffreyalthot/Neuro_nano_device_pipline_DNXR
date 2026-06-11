#pragma once

#include "neural_chip/NeuralChipPipelineStateMachine.h"

namespace dnxr {

struct HoldStabilitySnapshot {
    int holdFramesInCurrentState = 0;
    int stableHoldFrames = 0;
    const char* lastHoldFallback = "";
    const char* lastHoldReason = "";
};

class HoldStabilityTracker {
public:
    void observeHold(NeuralChipPipelineState state, const char* holdFallback, const char* reason, bool holdStillUnsafe);
    void applyTransitions(NeuralChipPipelineStateMachine& stateMachine, bool holdStillUnsafe, int stableFramesRequired = 3);
    const HoldStabilitySnapshot& snapshot() const;

private:
    NeuralChipPipelineState lastState_ = NeuralChipPipelineState::ColdStart;
    HoldStabilitySnapshot snapshot_{};
};

} // namespace dnxr
