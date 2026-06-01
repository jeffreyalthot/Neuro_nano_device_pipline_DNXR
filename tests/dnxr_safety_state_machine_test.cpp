#include "../src/safety/SafetyStateMachine.h"

#include <cassert>
#include <iostream>

int main() {
    dnxr::SafetyStateMachine machine;
    assert(machine.state() == dnxr::SafetyRuntimeState::BootSelfTest);

    machine.bootSelfTestPassed();
    assert(machine.state() == dnxr::SafetyRuntimeState::Nominal);

    dnxr::SafetyDecision degraded;
    degraded.action = dnxr::SafetyAction::ReduceIntensity;
    degraded.reason = "REQ-SAFE-STM-001:moderate_integrated_risk";
    machine.observeSafetyDecision(degraded);
    assert(machine.state() == dnxr::SafetyRuntimeState::Degraded);

    dnxr::SafetyDecision emergency;
    emergency.action = dnxr::SafetyAction::EmergencyStop;
    emergency.requiresEmergencyStop = true;
    emergency.reason = "REQ-SAFE-STM-002:autonomy_or_drift_emergency";
    machine.observeSafetyDecision(emergency);
    assert(machine.state() == dnxr::SafetyRuntimeState::EmergencyLatched);
    assert(machine.isEmergencyLatched());

    dnxr::SafetyDecision nominal;
    nominal.action = dnxr::SafetyAction::Continue;
    nominal.reason = "REQ-SAFE-STM-003:nominal_after_latch_must_not_clear";
    machine.observeSafetyDecision(nominal);
    assert(machine.state() == dnxr::SafetyRuntimeState::EmergencyLatched);

    machine.enterPostIncidentReview("operator:test-review");
    assert(machine.state() == dnxr::SafetyRuntimeState::PostIncidentReview);
    machine.resetToNominalAfterReview("operator:test-reset");
    assert(machine.state() == dnxr::SafetyRuntimeState::Nominal);

    assert(machine.auditTrail().size() >= 6U);
    std::cout << "Safety state machine latch semantics OK\n";
    return 0;
}
