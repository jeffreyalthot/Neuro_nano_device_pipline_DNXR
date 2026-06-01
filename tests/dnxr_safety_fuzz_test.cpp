#include "../src/safety/SafetyPipeline.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace {

float next01(std::mt19937& rng) {
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
}

std::vector<float> randomVector(std::mt19937& rng, std::size_t count) {
    std::vector<float> values;
    values.reserve(count);
    for (std::size_t i = 0U; i < count; ++i) {
        values.push_back(next01(rng));
    }
    return values;
}

dnxr::SafetyGovernorInput randomSafetyInput(std::mt19937& rng, std::uint32_t iteration) {
    dnxr::SafetyGovernorInput input;
    input.normalizedTelemetryRisk = randomVector(rng, 3U + (iteration % 5U));
    input.sensoryFrame = randomVector(rng, 8U + (iteration % 13U));
    input.inferredState = (iteration % 97U == 0U) ? "dissociation_risk" : "focused";

    input.cognitive.attention = next01(rng);
    input.cognitive.fatigue = next01(rng);
    input.cognitive.stress = next01(rng);
    input.cognitive.immersionIndex = next01(rng);

    input.biometrics.heartRate = 55.0f + (next01(rng) * 120.0f);
    input.biometrics.respirationRate = 10.0f + (next01(rng) * 30.0f);
    input.biometrics.galvanicSkinResponse = next01(rng);
    input.biometrics.eyeFocus = next01(rng);
    input.biometrics.postureStability = next01(rng);
    input.biometrics.movementEnergy = next01(rng);

    input.diagnostics.neuralSignalQuality = next01(rng);
    input.diagnostics.autonomyPreservation = next01(rng);
    input.diagnostics.decisionDrift = next01(rng);
    input.diagnostics.addictionRisk = next01(rng);
    input.diagnostics.addictionMitigationActive = (iteration % 41U == 0U);
    input.diagnostics.safetyMode = (iteration % 149U == 0U) ? "emergency_stop" : "nominal";
    input.diagnostics.intentAmbiguityDetected = (iteration % 17U == 0U);
    input.diagnostics.intentExecutionAuthorized = true;
    return input;
}

bool validDecisionRange(const dnxr::SafetyDecision& decision) {
    return std::isfinite(decision.riskScore) &&
           std::isfinite(decision.intensityScale) &&
           decision.riskScore >= 0.0f &&
           decision.riskScore <= 1.0f &&
           decision.intensityScale >= 0.0f &&
           decision.intensityScale <= 1.0f &&
           !decision.reason.empty() &&
           !decision.auditRecord.empty();
}

void assertTraceInvariant(const dnxr::SafetyDecisionTrace& trace, std::size_t expectedLayerCount) {
    assert(trace.layerDecisions.size() == expectedLayerCount);
    assert(trace.auditedEveryLayer());
    assert(validDecisionRange(trace.finalDecision));

    for (const auto& layerDecision : trace.layerDecisions) {
        assert(!layerDecision.layerName.empty());
        assert(!layerDecision.reason.empty());
        assert(std::isfinite(layerDecision.riskScore));
        assert(std::isfinite(layerDecision.intensityScale));
        assert(layerDecision.riskScore >= 0.0f);
        assert(layerDecision.riskScore <= 1.0f);
        assert(layerDecision.intensityScale >= 0.0f);
        assert(layerDecision.intensityScale <= 1.0f);
        if (layerDecision.requiresEmergencyStop) {
            assert(!layerDecision.allowRender);
        }
    }

    if (trace.finalDecision.requiresEmergencyStop) {
        assert(!trace.finalDecision.allowRender);
        assert(trace.finalDecision.action == dnxr::SafetyAction::EmergencyStop ||
               trace.finalDecision.action == dnxr::SafetyAction::Lockout);
    }
}

} // namespace

int main() {
    dnxr::SafetyPipeline pipeline = dnxr::SafetyPipeline::createDefault();
    assert(pipeline.layerCount() == 8U);

    std::mt19937 rng(0xD00D5AFE);
    std::uint32_t nonContinueCount = 0U;
    std::uint32_t emergencyCount = 0U;
    std::uint32_t renderBlockedCount = 0U;

    for (std::uint32_t iteration = 0U; iteration < 2048U; ++iteration) {
        const dnxr::SafetyDecisionTrace trace = pipeline.evaluate(randomSafetyInput(rng, iteration));
        assertTraceInvariant(trace, pipeline.layerCount());
        assert(trace.summarize().find("layers=8") != std::string::npos);

        if (trace.finalDecision.action != dnxr::SafetyAction::Continue) {
            ++nonContinueCount;
        }
        if (trace.finalDecision.requiresEmergencyStop) {
            ++emergencyCount;
        }
        if (!trace.finalDecision.allowRender) {
            ++renderBlockedCount;
        }
    }

    assert(nonContinueCount > 0U);
    assert(emergencyCount > 0U);
    assert(renderBlockedCount == emergencyCount);

    dnxr::SafetyGovernorInput forcedEmergency;
    forcedEmergency.normalizedTelemetryRisk = {0.99f, 1.0f, 0.98f};
    forcedEmergency.sensoryFrame = {0.9f, 0.9f, 0.9f};
    forcedEmergency.inferredState = "dissociation_risk";
    forcedEmergency.cognitive.attention = 0.05f;
    forcedEmergency.cognitive.fatigue = 1.0f;
    forcedEmergency.cognitive.stress = 1.0f;
    forcedEmergency.diagnostics.neuralSignalQuality = 0.05f;
    forcedEmergency.diagnostics.autonomyPreservation = 0.05f;
    forcedEmergency.diagnostics.decisionDrift = 0.99f;
    forcedEmergency.diagnostics.addictionRisk = 0.99f;
    forcedEmergency.diagnostics.safetyMode = "emergency_stop";

    const auto emergencyTrace = pipeline.evaluate(forcedEmergency);
    assertTraceInvariant(emergencyTrace, pipeline.layerCount());
    assert(emergencyTrace.finalDecision.action == dnxr::SafetyAction::EmergencyStop);
    assert(emergencyTrace.finalDecision.requiresEmergencyStop);
    assert(!emergencyTrace.finalDecision.allowRender);

    return 0;
}
