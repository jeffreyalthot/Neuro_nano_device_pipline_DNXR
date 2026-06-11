#include "neural_chip/NeuralChipFrameCommitter.h"
#include <cassert>

int main(){
    dnxr::NeuralChipBus bus; dnxr::FrameLifecycleGuard lifecycle; dnxr::SafeChipFrameHoldBuffer hold; dnxr::NeuralChipFrameCommitter committer(bus,lifecycle,hold);
    dnxr::FullDiveChipFrame f; f.frameId=1; f.timestampMs=1; f.touch.painBlocked01=f.touch.burnBlocked01=f.touch.crushBlocked01=f.touch.shockBlocked01=1.0f;
    auto ok=committer.commitCandidate(f); assert(ok.accepted); assert(!ok.safetyHold); assert(hold.hasAcceptedFrame());
    auto replay=committer.commitCandidate(f); assert(!replay.accepted); assert(replay.safetyHold); assert(bus.getHealthStatus().safeHoldActive);
}
