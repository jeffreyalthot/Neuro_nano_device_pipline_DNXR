#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); p.emergencyLatch().trigger("test emergency"); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; assert(!p.step(w)); dnxr::EmergencyLatchResetRequest bad; assert(!p.resetEmergencyLatchWithAudit(bad)); dnxr::EmergencyLatchResetRequest req{1,"op","audited",true,true}; assert(p.resetEmergencyLatchWithAudit(req)); assert(p.stateMachine().state()==dnxr::NeuralChipPipelineState::ControlledResume); }
