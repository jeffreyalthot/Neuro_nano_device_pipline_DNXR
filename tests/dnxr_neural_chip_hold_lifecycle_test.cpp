#include "neural_chip/NeuralChipFrameCommitter.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::FrameLifecycleGuard guard; dnxr::SafeChipFrameHoldBuffer buf; dnxr::NeuralChipFrameCommitter c(bus,guard,buf); dnxr::FullDiveChipFrame f; f.frameId=1; f.timestampMs=1; assert(c.commitCandidate(f).accepted); auto r=c.commitRejectedWithHold(f,"replay",0.0f); assert(r.safetyHold); assert(r.submittedFrame.frameId>f.frameId); assert(r.submittedFrame.timestampMs>f.timestampMs); }
