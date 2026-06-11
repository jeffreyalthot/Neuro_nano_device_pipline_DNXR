#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(p.step(w)); w.frameId=2; w.timestampMs=100; assert(!p.step(w)); const auto& r=p.replayRecorder().records().back(); assert(r.pipelinePathKind==dnxr::PipelinePathKind::AckMissingHold); assert(!r.commitResult.accepted || r.commitResult.safetyHold); }
