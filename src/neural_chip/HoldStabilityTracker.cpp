#include "neural_chip/HoldStabilityTracker.h"

namespace dnxr {

void HoldStabilityTracker::observeHold(NeuralChipPipelineState state, const char* holdFallback, const char* reason, bool holdStillUnsafe) {
    if (state != lastState_) {
        snapshot_.holdFramesInCurrentState = 0;
        snapshot_.stableHoldFrames = 0;
        lastState_ = state;
    }
    ++snapshot_.holdFramesInCurrentState;
    if (holdStillUnsafe) snapshot_.stableHoldFrames = 0;
    else ++snapshot_.stableHoldFrames;
    snapshot_.lastHoldFallback = holdFallback ? holdFallback : "";
    snapshot_.lastHoldReason = reason ? reason : "";
}

void HoldStabilityTracker::applyTransitions(NeuralChipPipelineStateMachine& stateMachine, bool holdStillUnsafe, int stableFramesRequired) {
    if (!stateMachine.inHold()) return;
    if (holdStillUnsafe && snapshot_.holdFramesInCurrentState >= 2) {
        stateMachine.onPersistentHold();
    } else if (!holdStillUnsafe && snapshot_.stableHoldFrames >= stableFramesRequired) {
        stateMachine.onStableHold();
    }
}

const HoldStabilitySnapshot& HoldStabilityTracker::snapshot() const { return snapshot_; }

} // namespace dnxr
