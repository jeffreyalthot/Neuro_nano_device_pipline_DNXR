#include "neural_chip/PipelineResumeStateBridge.h"
#include <cassert>
int main(){ dnxr::NeuralChipPipelineStateMachine sm; dnxr::PipelineResumeController rc; dnxr::PipelineResumeStateBridge b; sm.start(); sm.triggerEmergency("x"); b.synchronizeBeforeDecision(sm,rc,true); assert(rc.locked()); sm.onAuditedReset(); b.synchronizeBeforeDecision(sm,rc,false); auto d=rc.decide(); assert(!d.allowNormalFrame); }
