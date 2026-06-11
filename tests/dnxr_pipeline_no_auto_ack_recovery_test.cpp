#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){
    dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1;
    auto a=p.stepDetailed(w); assert(a.outcome==dnxr::PipelineStepOutcome::NormalAccepted); assert(p.stateMachine().state()==dnxr::NeuralChipPipelineState::AwaitingAck);
    w.frameId=2; w.timestampMs=2; auto wait=p.stepDetailed(w); assert(wait.outcome==dnxr::PipelineStepOutcome::WaitingForAck); assert(p.stateMachine().state()==dnxr::NeuralChipPipelineState::AwaitingAck); assert(!bus.getHealthStatus().safeHoldActive);
    p.injectSimulatedAck(1,1.0f); assert(p.stateMachine().state()==dnxr::NeuralChipPipelineState::Running);
}
