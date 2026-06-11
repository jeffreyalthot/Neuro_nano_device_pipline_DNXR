#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>

int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(p.step(w)); assert(bus.lastAcceptedFrame().olfactory.odorPresence01==0.0f); assert(bus.lastAcceptedFrame().gustatory.sweet01==0.0f); assert(!p.step(w)); assert(bus.getHealthStatus().safeHoldActive); w.frameId=2; w.timestampMs=0.5; assert(!p.step(w)); }
