#include "neural_chip/HoldFrameSequencer.h"
#include <cassert>
int main(){ dnxr::HoldFrameSequencer s; s.observeCommitted(10,10.0); auto a=s.nextHold(9.0,"neutral"); assert(a.frameId==11); assert(a.timestampMs>10.0); assert(a.holdKind[0]=='n'); s.observeCommitted(a.frameId,a.timestampMs); auto b=s.nextHold(200.0,"decompression"); assert(b.frameId==12); assert(b.timestampMs>a.timestampMs); assert(b.timestampMs-a.timestampMs<=100.0); }
