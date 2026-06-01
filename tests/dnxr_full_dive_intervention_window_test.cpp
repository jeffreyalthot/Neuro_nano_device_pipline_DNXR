#include "../src/runtime/FullDiveSafetyBridge.h"
#include "../src/runtime/FullDiveRuntimeInput.h"

#include <cassert>

namespace {

dnxr::FullDiveRuntimeInput desynchronizedInput() {
    dnxr::FullDiveRuntimeInput input;
    input.cognitive.immersionIndex = 0.5f;
    input.cognitive.emotionValence = 0.2f;
    input.biometric.postureStability = 0.8f;
    input.biometric.eyeFocus = 0.8f;
    input.diagnostics.neuralSignalQuality = 0.9f;
    input.diagnostics.intentConfidence = 0.9f;
    input.calibrationDrift.driftScore = 0.05f;
    input.safetyDecision.intensityScale = 1.0f;
    input.realityAnchorPlan.allowRender = true;
    input.sensorySync.confidence = 0.9f;
    input.sensorySync.recommendedIntensityScale = 0.9f;
    input.sensorySync.synchronized = false;
    input.runtimeMetrics.latencyBudgetMs = 20.0;
    input.runtimeMetrics.lastLatencyMs = 5.0;
    input.deltaTimeSeconds = 0.016;
    return input;
}

} // namespace

int main() {
    dnxr::FullDiveSynchronizationEngine engine;
    engine.setUpdateMode(dnxr::FullDiveUpdateMode::RuntimeDriven);

    dnxr::SafetyRuntimeProfile profile;
    profile.minFullDiveStability = 0.10f;
    profile.minTemporalCoherence = 0.90f;
    profile.minVestibularStability = 0.10f;
    profile.minMotorPredictionAccuracy = 0.10f;
    engine.configureSafetyThresholds(profile);

    dnxr::FullDiveInterventionMemory memory;
    memory.windowSeconds = 60.0;
    memory.cooldownSeconds = 0.0;
    dnxr::FullDiveSafetyBridge bridge(memory);

    auto input = desynchronizedInput();

    engine.updateFromRuntime(input);
    const auto first = bridge.evaluate(engine);
    assert(first.safetyInterventionRequired());
    assert(first.memory.reduceIntensityCount + first.memory.decompressionCount +
           first.memory.realityAnchorCount + first.memory.emergencyStopCount >= 1U);

    engine.updateFromRuntime(input);
    const auto second = bridge.evaluate(engine);
    assert(second.safetyInterventionRequired());
    assert(second.memory.reduceIntensityCount + second.memory.decompressionCount +
           second.memory.realityAnchorCount + second.memory.emergencyStopCount >= 2U);

    engine.updateFromRuntime(input);
    const auto third = bridge.evaluate(engine);
    assert(third.safetyInterventionRequired());

    input.sensorySync.synchronized = false;
    input.runtimeMetrics.lastLatencyMs = 100.0;
    input.biometric.postureStability = 0.0f;
    input.biometric.eyeFocus = 0.0f;

    dnxr::FullDiveRuntimeSnapshot fourth;
    for (int i = 0; i < 4; ++i) {
        engine.updateFromRuntime(input);
        fourth = bridge.evaluate(engine);
    }
    assert(fourth.intervention == dnxr::FullDiveInterventionLevel::Lockout);
    assert(fourth.memory.lockoutCount >= 1U);

    dnxr::SafetyDecision base;
    base.allowRender = true;
    base.intensityScale = 1.0f;
    base.riskScore = 0.2f;
    base.reason = "base";
    const auto fused = bridge.mergeIntoDecision(fourth, base);
    assert(!fused.allowRender);
    assert(fused.requiresEmergencyStop);
    assert(fused.action == dnxr::SafetyAction::Lockout);
    assert(fused.riskScore >= fourth.severity);
    return 0;
}
