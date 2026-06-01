#include "../src/runtime/FullDiveSafetyBridge.h"
#include "../src/runtime/FullDiveRuntimeInput.h"

#include <cassert>
#include <chrono>

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
    const auto first = bridge.evaluateAndRemember(engine);
    assert(first.safetyInterventionRequired());
    assert(first.memory.reduceIntensityCount + first.memory.decompressionCount +
           first.memory.realityAnchorCount + first.memory.emergencyStopCount >= 1U);

    engine.updateFromRuntime(input);
    const auto second = bridge.evaluateAndRemember(engine);
    assert(second.safetyInterventionRequired());
    assert(second.memory.reduceIntensityCount + second.memory.decompressionCount +
           second.memory.realityAnchorCount + second.memory.emergencyStopCount >= 2U);

    engine.updateFromRuntime(input);
    const auto third = bridge.evaluateAndRemember(engine);
    assert(third.safetyInterventionRequired());

    input.sensorySync.synchronized = false;
    input.runtimeMetrics.lastLatencyMs = 100.0;
    input.biometric.postureStability = 0.0f;
    input.biometric.eyeFocus = 0.0f;

    dnxr::FullDiveRuntimeSnapshot fourth;
    for (int i = 0; i < 4; ++i) {
        engine.updateFromRuntime(input);
        fourth = bridge.evaluateAndRemember(engine);
    }
    assert(fourth.intervention == dnxr::FullDiveInterventionLevel::Lockout);
    assert(fourth.memory.lockoutCount >= 1U);

    dnxr::SafetyDecision base;
    base.allowRender = true;
    base.intensityScale = 1.0f;
    base.riskScore = 0.2f;
    base.reason = "base";
    dnxr::FullDiveSafetyBridge deterministicBridge(memory);
    dnxr::FullDiveRuntimeSnapshot instant;
    instant.intervention = dnxr::FullDiveInterventionLevel::RealityAnchor;
    instant.severity = 0.80f;
    instant.reason = "deterministic_anchor";
    instant.traceTag = "full_dive:test_anchor";
    instant.state.vestibularStability = 0.90f;
    const auto t0 = std::chrono::steady_clock::time_point{} + std::chrono::seconds(1);
    const auto d1 = deterministicBridge.observeAndEscalate(instant, t0);
    const auto d2 = deterministicBridge.observeAndEscalate(instant, t0 + std::chrono::seconds(20));
    const auto d3 = deterministicBridge.observeAndEscalate(instant, t0 + std::chrono::seconds(40));
    assert(d1.intervention == dnxr::FullDiveInterventionLevel::RealityAnchor);
    assert(d2.intervention == dnxr::FullDiveInterventionLevel::RealityAnchor);
    assert(d3.intervention == dnxr::FullDiveInterventionLevel::EmergencyStop);

    const auto fused = bridge.mergeIntoDecision(fourth, base);
    assert(!fused.allowRender);
    assert(fused.requiresEmergencyStop);
    assert(fused.action == dnxr::SafetyAction::Lockout);
    assert(fused.riskScore >= fourth.severity);
    return 0;
}
