#include "neural_chip/FrameLifecycleGuard.h"
#include <cassert>
int main(){ dnxr::FrameLifecycleGuard g; dnxr::FullDiveChipFrame f; f.frameId=1; f.timestampMs=1; assert(g.preview(f).accepted); assert(g.preview(f).accepted); g.commitAccepted(f); assert(!g.preview(f).accepted); dnxr::FullDiveChipFrame n=f; n.frameId=2; n.timestampMs=2; assert(g.preview(n).accepted); }
