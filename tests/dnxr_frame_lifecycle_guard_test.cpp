#include "neural_chip/FrameLifecycleGuard.h"
#include <cassert>
int main(){ dnxr::FrameLifecycleGuard guard; dnxr::FullDiveChipFrame f; f.frameId=1; f.timestampMs=10; assert(guard.validate(f).accepted); assert(!guard.validate(f).accepted); f.frameId=2; f.timestampMs=9; assert(!guard.validate(f).accepted); guard.reset(); dnxr::FrameLifecyclePolicy p; p.nowMs=1000; dnxr::FrameLifecycleGuard g(p); f.frameId=3; f.timestampMs=700; assert(!g.validate(f).accepted); }
