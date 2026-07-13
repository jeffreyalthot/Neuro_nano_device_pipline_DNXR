#include "../src/safety/SafetyPipeline.h"
#include "../src/safety/SafetyStateMachine.h"

#include <cassert>
#include <string>

namespace {

dnxr::SafetyGovernorInput nominalInput() {
    dnxr::SafetyGovernorInput input{};
    input.normalizedTelemetryRisk = {0.05F, 0.05F, 0.05F};
    input.sensoryFrame = {0.10F, 0.10F, 0.10F};
    input.inferredState = "focused";
    input.cognitive.attention = 0.90F;
    input.cognitive.stress = 0.05F;
    input.cognitive.fatigue = 0.05F;
    input.diagnostics.neuralSignalQuality = 0.95F;
    input.diagnostics.autonomyPreservation = 0.98F;
    input.diagnostics.decisionDrift = 0.02F;
    input.diagnostics.addictionRisk = 0.01F;
    input.diagnostics.safetyMode = "nominal";
    return input;
}

} // namespace

int main() {
    auto pipeline = dnxr::SafetyPipeline::createDefault();
    auto input = nominalInput();
    input.diagnostics.neuralSignalQuality = 0.10F; // transport/hardware hold cause
    input.diagnostics.decisionDrift = 0.98F;       // later emergency cause, must dominate

    const auto trace = pipeline.evaluate(input);
    assert(trace.auditedEveryLayer());
    assert(trace.finalDecision.action == dnxr::SafetyAction::EmergencyStop);
    assert(trace.finalDecision.requiresEmergencyStop);
    assert(trace.finalDecision.intensityScale == 0.0F);
    assert(!trace.finalDecision.allowRender);
    assert(trace.finalDecision.reason.find("cross_modal:RealityAnchorLayer") != std::string::npos ||
           trace.finalDecision.reason.find("EmergencyLayer") != std::string::npos);
    assert(trace.finalDecision.auditRecord.find("transport_hardware/SignalIntegrityLayer=decompress") != std::string::npos);
    assert(trace.finalDecision.auditRecord.find("cross_modal/RealityAnchorLayer=emergency_stop") != std::string::npos);

    dnxr::SafetyStateMachine machine;
    assert(machine.state() == dnxr::SafetyRuntimeState::Booting);
    machine.bootSelfTestPassed();
    assert(machine.state() == dnxr::SafetyRuntimeState::EvidenceCheck);
    machine.evidenceAccepted("signed-evidence-test");
    machine.connected("canonical-transport-test");
    machine.calibrated("calibration-test");
    assert(machine.state() == dnxr::SafetyRuntimeState::Ready);
    machine.startRunning("controlled-start-test");
    machine.awaitAck("canonical-dispatch-test");
    machine.observeSafetyDecision(trace.finalDecision);
    assert(machine.state() == dnxr::SafetyRuntimeState::EmergencyLatched);
    machine.enterPostIncidentReview("operator:test-review");
    machine.resetToNominalAfterReview("operator:test-reset");
    assert(machine.state() == dnxr::SafetyRuntimeState::ControlledResume);
    machine.startRunning("controlled-resume-test");
    assert(machine.state() == dnxr::SafetyRuntimeState::Running);

    const auto audit = machine.auditTrail();
    assert(audit.size() >= 10U);
    for (const auto& transition : audit) {
        assert(!transition.reason.empty());
        assert(!transition.authority.empty());
    }

    return 0;
}
