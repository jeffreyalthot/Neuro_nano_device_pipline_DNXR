#include "FullDiveSafetyBridge.h"

namespace dnxr {

FullDiveRuntimeSnapshot FullDiveSafetyBridge::evaluate(const FullDiveSynchronizationEngine& engine) const {
    FullDiveRuntimeSnapshot snapshot;
    snapshot.state = engine.getState();
    snapshot.safetyInterventionRequired = snapshot.state.emergencyExitTriggered ||
                                          snapshot.state.neuralOverloadDetected ||
                                          snapshot.state.consciousnessDesynchronization;
    if (snapshot.state.emergencyExitTriggered) {
        snapshot.reason = "full_dive_emergency_exit";
    } else if (snapshot.state.neuralOverloadDetected) {
        snapshot.reason = "full_dive_neural_overload";
    } else if (snapshot.state.consciousnessDesynchronization) {
        snapshot.reason = "full_dive_consciousness_desynchronization";
    }
    return snapshot;
}

void FullDiveSafetyBridge::apply(const FullDiveRuntimeSnapshot& snapshot, SafetyStateMachine& stateMachine) const {
    if (snapshot.safetyInterventionRequired) {
        stateMachine.latchEmergency(snapshot.reason);
    }
}

} // namespace dnxr
