#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=-1; assert(!p.step(w)); assert(p.replayRecorder().records().back().pipelinePathKind==dnxr::PipelinePathKind::WorldFrameReject); }
