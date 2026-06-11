#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(p.step(w)); assert(p.replayRecorder().records().size()==1); const auto& r=p.replayRecorder().records().back(); assert(r.frameId==1); assert(r.commitResult.accepted); assert(!r.emergencyLatched); }
