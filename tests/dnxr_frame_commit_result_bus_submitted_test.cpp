#include "neural_chip/NeuralChipFrameCommitter.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::FrameLifecycleGuard lifecycle; dnxr::SafeChipFrameHoldBuffer hold; dnxr::NeuralChipFrameCommitter c(bus,lifecycle,hold); dnxr::FullDiveChipFrame f; f.frameId=1; f.timestampMs=1; f.visual.globalLuminance01=2.0f; auto r=c.commitCandidate(f); assert(!r.candidateAccepted); assert(r.busSubmitted); assert(r.safetyHold); assert(r.holdFrameSubmitted || bus.getHealthStatus().safeHoldActive); }
