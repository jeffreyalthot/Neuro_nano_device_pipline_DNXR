#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "neural_chip/DecisionTraceCoverageValidator.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(p.step(w)); auto path=p.replayRecorder().records().back().pipelinePathKind; assert(dnxr::DecisionTraceCoverageValidator().validate(p.decisionTrace(), path).complete); }
