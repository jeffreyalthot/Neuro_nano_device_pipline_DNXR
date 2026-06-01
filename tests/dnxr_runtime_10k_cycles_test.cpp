#include "../src/runtime/RuntimeState.h"
#include "../src/safety/SafetyPipeline.h"
#include "../src/simulation/ScenarioRunner.h"
#include "../src/utils/RuntimeMetrics.h"
#include "../src/utils/SessionSafetyJournal.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace {

bool finiteVector(const std::vector<float>& values) {
    return std::all_of(values.begin(), values.end(), [](float value) {
        return std::isfinite(value);
    });
}

std::string safetyActionName(dnxr::SafetyAction action) {
    switch (action) {
    case dnxr::SafetyAction::Continue:
        return "continue";
    case dnxr::SafetyAction::ReduceIntensity:
        return "reduce_intensity";
    case dnxr::SafetyAction::Decompress:
        return "decompress";
    case dnxr::SafetyAction::RealityAnchor:
        return "reality_anchor";
    case dnxr::SafetyAction::EmergencyStop:
        return "emergency_stop";
    case dnxr::SafetyAction::Lockout:
        return "lockout";
    }
    return "unknown";
}

dnxr::BiometricFrame toBiometricFrame(const dnxr::BiometricSample& sample) {
    dnxr::BiometricFrame frame;
    frame.heartRate = sample.heartRateBpm;
    frame.respirationRate = sample.respiratoryRateRpm;
    frame.galvanicSkinResponse = sample.neuralNoise;
    frame.eyeFocus = sample.eyeStability;
    frame.postureStability = sample.oxygenLevel;
    frame.movementEnergy = 1.0f - sample.focusIndex;
    return frame;
}

dnxr::CognitiveState toCognitiveState(const dnxr::BiometricSample& sample) {
    dnxr::CognitiveState state;
    state.attention = sample.focusIndex;
    state.fatigue = std::clamp((sample.heartRateBpm - 72.0f) / 70.0f, 0.0f, 1.0f);
    state.stress = std::clamp(sample.neuralNoise * 2.0f, 0.0f, 1.0f);
    state.immersionIndex = std::clamp(sample.focusIndex * sample.eyeStability, 0.0f, 1.0f);
    return state;
}

dnxr::SafetyGovernorInput toSafetyInput(const dnxr::SimulationFrame& frame) {
    dnxr::SafetyGovernorInput input;
    input.normalizedTelemetryRisk = {
        std::clamp((frame.biometrics.heartRateBpm - 60.0f) / 90.0f, 0.0f, 1.0f),
        std::clamp(frame.biometrics.neuralNoise, 0.0f, 1.0f),
        std::clamp(1.0f - frame.biometrics.oxygenLevel, 0.0f, 1.0f),
    };
    input.sensoryFrame = frame.neuralSignal;
    input.inferredState = frame.safetyStressExpected ? "stress_monitor" : "focused";
    input.cognitive = toCognitiveState(frame.biometrics);
    input.biometrics = toBiometricFrame(frame.biometrics);
    input.diagnostics.streamSource = "deterministic_10k_simulation";
    input.diagnostics.neuralSignalQuality = frame.dropoutActive ? 0.10f : std::clamp(1.0f - frame.biometrics.neuralNoise, 0.0f, 1.0f);
    input.diagnostics.autonomyPreservation = 0.95f;
    input.diagnostics.decisionDrift = frame.safetyStressExpected ? 0.35f : 0.05f;
    input.diagnostics.addictionRisk = frame.scenarioName == "addiction_risk_pattern" ? 0.80f : 0.10f;
    input.diagnostics.addictionMitigationActive = frame.scenarioName == "addiction_risk_pattern";
    input.diagnostics.safetyMode = "nominal";
    return input;
}

} // namespace

int main() {
    constexpr std::uint64_t kCycleCount = 10000U;
    constexpr std::size_t kHistoryLimit = 512U;

    dnxr::NeuralSignalSimulatorConfig config;
    config.scenario = dnxr::SimulationScenario::SignalDropout;
    config.channelCount = 16U;
    config.noiseAmplitude = 0.04f;
    config.dropoutEveryCycles = 97U;

    dnxr::ScenarioRunner runner(config);
    dnxr::SafetyPipeline safetyPipeline = dnxr::SafetyPipeline::createDefault();
    dnxr::RuntimeMetrics metrics(5.0, 256U);
    dnxr::RuntimeState state(kHistoryLimit);
    dnxr::SessionSafetyJournal journal;

    std::uint64_t dropoutFrames = 0U;
    std::uint64_t nonContinueSafetyFrames = 0U;
    std::uint64_t emergencyFrames = 0U;
    std::string firstFrameHash;
    std::string lastFrameHash;

    for (std::uint64_t ordinal = 1U; ordinal <= kCycleCount; ++ordinal) {
        const dnxr::SimulationFrame frame = runner.nextFrame();
        assert(frame.cycle == ordinal - 1U);
        assert(!frame.neuralSignal.empty());
        assert(finiteVector(frame.neuralSignal));
        assert(finiteVector(frame.biometrics.toTelemetryVector()));

        const dnxr::SafetyDecisionTrace trace = safetyPipeline.evaluate(toSafetyInput(frame));
        assert(trace.layerDecisions.size() == safetyPipeline.layerCount());
        assert(trace.auditedEveryLayer());
        assert(!trace.finalDecision.auditRecord.empty());

        if (frame.dropoutActive) {
            ++dropoutFrames;
        }
        if (trace.finalDecision.action != dnxr::SafetyAction::Continue) {
            ++nonContinueSafetyFrames;
        }
        if (trace.finalDecision.requiresEmergencyStop) {
            ++emergencyFrames;
        }

        const double syntheticLatencyMs = frame.dropoutActive ? 7.0 : (1.0 + static_cast<double>(ordinal % 4U) * 0.25);
        metrics.recordCycleMeasurement(
            syntheticLatencyMs,
            trace.finalDecision.allowRender,
            trace.finalDecision.requiresEmergencyStop,
            trace.finalDecision.reason);
        const dnxr::RuntimeMetricsSnapshot snapshot = metrics.snapshot();

        dnxr::RuntimeFrame runtimeFrame;
        runtimeFrame.cycleId = frame.cycle;
        runtimeFrame.timestampMs = static_cast<std::int64_t>(frame.cycle * 16U);
        runtimeFrame.rawSignal = frame.neuralSignal;
        runtimeFrame.cleanSignal = frame.neuralSignal;
        runtimeFrame.features = frame.biometrics.toTelemetryVector();
        runtimeFrame.cognitiveState = frame.safetyStressExpected ? "stress_monitor" : "focused";
        runtimeFrame.biometrics = frame.biometrics.toTelemetryVector();
        runtimeFrame.xrAction = frame.dropoutActive ? "none" : "xr.look";
        runtimeFrame.xrFrame = trace.finalDecision.allowRender ? frame.neuralSignal : std::vector<float>{};
        runtimeFrame.safetyDecision = safetyActionName(trace.finalDecision.action);
        runtimeFrame.metricsSnapshot = snapshot;
        state.commitFrame(runtimeFrame);

        const std::string frameHash = dnxr::runtimeFrameHash(runtimeFrame);
        assert(!frameHash.empty());
        if (ordinal == 1U) {
            firstFrameHash = frameHash;
        }
        lastFrameHash = frameHash;

        const auto auditEvent = journal.record(
            "runtime_10k_cycle",
            trace.finalDecision.auditRecord,
            trace.finalDecision.riskScore,
            {frame.cycle, runtimeFrame.safetyDecision, trace.finalDecision.allowRender, trace.finalDecision.requiresEmergencyStop});
        assert(auditEvent.sequenceId == ordinal);
        assert(auditEvent.runtimeCycle == frame.cycle);
        assert(!auditEvent.eventHash.empty());
        assert(auditEvent.safetyAction == runtimeFrame.safetyDecision);
    }

    const dnxr::RuntimeMetricsSnapshot finalSnapshot = metrics.snapshot();
    assert(finalSnapshot.totalCycles == kCycleCount);
    assert(finalSnapshot.p95LatencyMs <= 5.0);
    assert(finalSnapshot.p99LatencyMs >= 7.0);
    assert(finalSnapshot.latencyBudgetExceededCount == dropoutFrames);
    assert(finalSnapshot.safetyEventCount == emergencyFrames);
    assert(dropoutFrames >= 100U);
    assert(nonContinueSafetyFrames >= dropoutFrames);
    assert(emergencyFrames == 0U);

    assert(state.hasFrame());
    assert(state.history().size() == kHistoryLimit);
    assert(state.currentFrame().cycleId == kCycleCount - 1U);
    assert(state.currentFrame().metricsSnapshot.totalCycles == kCycleCount);
    assert(firstFrameHash != lastFrameHash);

    assert(journal.events().size() == kCycleCount);
    assert(journal.verifyChain());
    assert(journal.latestHash() == journal.events().back().eventHash);
    assert(journal.events().front().previousHash == "genesis");

    return 0;
}
