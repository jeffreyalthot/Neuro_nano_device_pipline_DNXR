#include "../src/runtime/FullDiveSafetyBridge.h"
#include "../src/runtime/FullDiveRuntimeInput.h"
#include "../src/safety/SafetyStateMachine.h"

#include <cassert>

int main() {
    dnxr::FullDiveSynchronizationEngine engine;
    engine.setUpdateMode(dnxr::FullDiveUpdateMode::RuntimeDriven);

    dnxr::SafetyRuntimeProfile profile;
    profile.minFullDiveStability = 0.30f;
    profile.minTemporalCoherence = 0.25f;
    profile.minVestibularStability = 0.25f;
    profile.minMotorPredictionAccuracy = 0.25f;
    engine.configureSafetyThresholds(profile);

    dnxr::FullDiveRuntimeInput input;
    input.cognitive.immersionIndex = 0.25f;
    input.cognitive.emotionValence = 0.2f;
    input.biometric.postureStability = 0.8f;
    input.biometric.eyeFocus = 0.8f;
    input.diagnostics.neuralSignalQuality = 0.25f;
    input.diagnostics.intentConfidence = 0.45f;
    input.calibrationDrift.driftScore = 0.1f;
    input.safetyDecision.intensityScale = 1.0f;
    input.realityAnchorPlan.allowRender = true;
    input.sensorySync.confidence = 0.35f;
    input.sensorySync.recommendedIntensityScale = 0.8f;
    input.sensorySync.synchronized = true;
    input.runtimeMetrics.latencyBudgetMs = 20.0;
    input.runtimeMetrics.lastLatencyMs = 5.0;
    input.deltaTimeSeconds = 0.016;

    engine.updateFromRuntime(input);
    dnxr::FullDiveSafetyBridge bridge;
    const auto reduce = bridge.evaluateAndRemember(engine);
    assert(reduce.safetyInterventionRequired());
    assert(reduce.intervention == dnxr::FullDiveInterventionLevel::ReduceIntensity);

    dnxr::SafetyStateMachine machine;
    machine.bootSelfTestPassed();
    bridge.apply(reduce, machine);
    assert(machine.state() == dnxr::SafetyRuntimeState::Degraded);

    input.sensorySync.synchronized = false;
    input.runtimeMetrics.lastLatencyMs = 100.0;
    input.biometric.postureStability = 0.0f;
    input.biometric.eyeFocus = 0.0f;
    engine.updateFromRuntime(input);
    const auto anchor = bridge.evaluateAndRemember(engine);
    assert(anchor.safetyInterventionRequired());
    assert(anchor.intervention == dnxr::FullDiveInterventionLevel::RealityAnchor ||
           anchor.intervention == dnxr::FullDiveInterventionLevel::EmergencyStop ||
           anchor.intervention == dnxr::FullDiveInterventionLevel::Lockout);
    return 0;
}
