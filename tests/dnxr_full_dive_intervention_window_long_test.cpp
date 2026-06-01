#include "../src/runtime/FullDiveSafetyBridge.h"
#include "../src/runtime/FullDiveRuntimeInput.h"

#include <cassert>

int main() {
    dnxr::FullDiveSynchronizationEngine engine;
    engine.setUpdateMode(dnxr::FullDiveUpdateMode::RuntimeDriven);
    dnxr::SafetyRuntimeProfile profile;
    profile.minTemporalCoherence = 0.95f;
    engine.configureSafetyThresholds(profile);
    dnxr::FullDiveInterventionMemory memory;
    memory.cooldownSeconds = 0.0;
    memory.windowSeconds = 3600.0;
    dnxr::FullDiveSafetyBridge bridge(memory);
    dnxr::FullDiveRuntimeInput input;
    input.cognitive.immersionIndex = 0.6f;
    input.biometric.eyeFocus = 0.0f;
    input.biometric.postureStability = 0.0f;
    input.diagnostics.neuralSignalQuality = 0.9f;
    input.diagnostics.intentConfidence = 0.9f;
    input.sensorySync.synchronized = false;
    input.sensorySync.confidence = 0.9f;
    input.runtimeMetrics.latencyBudgetMs = 10.0;
    input.runtimeMetrics.lastLatencyMs = 250.0;
    dnxr::FullDiveRuntimeSnapshot snapshot;
    for (int i = 0; i < 8; ++i) {
        engine.updateFromRuntime(input);
        snapshot = bridge.evaluateAndRemember(engine);
    }
    assert(snapshot.intervention == dnxr::FullDiveInterventionLevel::Lockout);
    return 0;
}
