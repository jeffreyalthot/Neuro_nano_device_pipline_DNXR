#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){
    dnxr::NeuralChipBus bus1; dnxr::NeuralChipOnlyFullDivePipeline p1(bus1); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(p1.step(w)); p1.injectSimulatedAck(1,0.9f); w.frameId=2; w.timestampMs=100; assert(p1.step(w));
    dnxr::NeuralChipBus bus2; dnxr::NeuralChipOnlyFullDivePipeline p2(bus2); w.frameId=1; w.timestampMs=1; assert(p2.step(w)); w.frameId=2; w.timestampMs=100; assert(!p2.step(w)); assert(bus2.getHealthStatus().safeHoldActive); assert(bus2.lastAcceptedFrame().frameId>1);
}
