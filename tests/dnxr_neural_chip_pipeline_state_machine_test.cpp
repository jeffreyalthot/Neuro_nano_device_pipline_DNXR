#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipPipelineStateMachine sm;
    assert(sm.state() == NeuralChipPipelineState::ColdStart);
    assert(sm.start());
    assert(sm.allowsNormalFrame());
    assert(sm.onCommitAccepted());
    assert(!sm.allowsNormalFrame());
    assert(sm.onAckMissingHold());
    assert(sm.inHold());
    assert(sm.onPersistentHold());
    assert(sm.onPersistentHold());
    assert(sm.onStableHold());
    assert(sm.onResumeRampComplete());
    assert(sm.triggerEmergency("critical"));
    assert(sm.emergencyLatched());
    assert(sm.onAuditedReset());
    assert(sm.state() == NeuralChipPipelineState::ControlledResume);
    assert(sm.shutdown());
    assert(!sm.start());
    return 0;
}
