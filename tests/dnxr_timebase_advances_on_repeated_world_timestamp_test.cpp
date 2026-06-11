#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){
    dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1;
    assert(p.stepDetailed(w).outcome==dnxr::PipelineStepOutcome::NormalAccepted); assert(p.stateMachine().state()==dnxr::NeuralChipPipelineState::AwaitingAck);
    dnxr::PipelineStepOutcome last=dnxr::PipelineStepOutcome::WaitingForAck;
    for(int i=0;i<8 && last==dnxr::PipelineStepOutcome::WaitingForAck;++i){ w.frameId++; auto r=p.stepDetailed(w); last=r.outcome; }
    assert(last==dnxr::PipelineStepOutcome::HoldCommitted || last==dnxr::PipelineStepOutcome::EmergencyLatched); assert(p.sessionState().holdFrames>0);
}
