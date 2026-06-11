#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); p.shutdown("operator shutdown"); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=7; w.timestampMs=7; auto r=p.stepDetailed(w); assert(r.outcome==dnxr::PipelineStepOutcome::Shutdown); assert(r.path==dnxr::PipelinePathKind::ShutdownNoOp); assert(!r.busSubmitted); assert(p.replayRecorder().records().back().noOp); }
