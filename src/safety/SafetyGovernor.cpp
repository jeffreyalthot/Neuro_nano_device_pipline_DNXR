#include "SafetyGovernor.h"

#include "EthicalGuardEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

} // namespace

float SafetyGovernor::computeTelemetryRisk(
    const std::vector<float>& normalizedTelemetryRisk) const {

    if (normalizedTelemetryRisk.empty()) {
        return 0.0f;
    }

    const float total = std::accumulate(
        normalizedTelemetryRisk.begin(),
        normalizedTelemetryRisk.end(),
        0.0f);

    return clamp01(total / static_cast<float>(normalizedTelemetryRisk.size()));
}

float SafetyGovernor::computeSensoryLoad(
    const std::vector<float>& sensoryFrame) const {

    if (sensoryFrame.empty()) {
        return 0.0f;
    }

    float load = 0.0f;
    for (float channel : sensoryFrame) {
        load += std::abs(channel);
    }

    return clamp01(load / static_cast<float>(sensoryFrame.size()));
}

std::string SafetyGovernor::actionName(SafetyAction action) const {
    switch (action) {
    case SafetyAction::Continue:
        return "continue";
    case SafetyAction::ReduceIntensity:
        return "reduce_intensity";
    case SafetyAction::Decompress:
        return "decompress";
    case SafetyAction::RealityAnchor:
        return "reality_anchor";
    case SafetyAction::EmergencyStop:
        return "emergency_stop";
    case SafetyAction::Lockout:
        return "lockout";
    }

    return "unknown";
}

SafetyDecision SafetyGovernor::evaluate(const SafetyGovernorInput& input) const {
    EthicalGuardEngine ethicalGuard;
    SafetyDecision decision;

    const float telemetryRisk = computeTelemetryRisk(input.normalizedTelemetryRisk);
    const float sensoryLoad = computeSensoryLoad(input.sensoryFrame);
    const float signalRisk = clamp01(1.0f - input.diagnostics.neuralSignalQuality);
    const float autonomyRisk = clamp01(1.0f - input.diagnostics.autonomyPreservation);
    const float driftRisk = clamp01(input.diagnostics.decisionDrift);
    const float addictionRisk = clamp01(input.diagnostics.addictionRisk);
    const float cognitiveRisk = clamp01(
        (input.cognitive.stress * 0.45f) +
        (input.cognitive.fatigue * 0.30f) +
        ((1.0f - input.cognitive.attention) * 0.25f));

    decision.riskScore = clamp01(
        (telemetryRisk * 0.20f) +
        (cognitiveRisk * 0.20f) +
        (signalRisk * 0.15f) +
        (autonomyRisk * 0.15f) +
        (driftRisk * 0.15f) +
        (addictionRisk * 0.10f) +
        (sensoryLoad * 0.05f));

    const bool ethicalAllowed = ethicalGuard.enforceNonBypassableLimits(
        input.normalizedTelemetryRisk,
        input.inferredState);

    if (!ethicalAllowed || input.inferredState.find("dissociation") != std::string::npos) {
        decision.action = SafetyAction::EmergencyStop;
        decision.reason = "ethical_or_dissociation_limit";
    } else if (input.diagnostics.autonomyPreservation < 0.20f ||
               input.diagnostics.decisionDrift > 0.85f ||
               decision.riskScore >= 0.90f) {
        decision.action = SafetyAction::EmergencyStop;
        decision.reason = "autonomy_or_drift_emergency";
    } else if (input.diagnostics.addictionMitigationActive ||
               input.diagnostics.addictionRisk > 0.75f) {
        decision.action = SafetyAction::RealityAnchor;
        decision.reason = "addiction_risk_reality_anchor";
    } else if (decision.riskScore >= 0.70f ||
               input.diagnostics.safetyMode == "stability_guard") {
        decision.action = SafetyAction::Decompress;
        decision.reason = "high_integrated_risk";
    } else if (decision.riskScore >= 0.50f ||
               input.diagnostics.safetyMode == "degraded_fallback") {
        decision.action = SafetyAction::ReduceIntensity;
        decision.reason = "moderate_integrated_risk";
    } else {
        decision.action = SafetyAction::Continue;
        decision.reason = "nominal";
    }

    switch (decision.action) {
    case SafetyAction::Continue:
        decision.intensityScale = 1.0f;
        decision.allowRender = true;
        decision.requiresEmergencyStop = false;
        break;
    case SafetyAction::ReduceIntensity:
        decision.intensityScale = 0.70f;
        decision.allowRender = true;
        decision.requiresEmergencyStop = false;
        break;
    case SafetyAction::Decompress:
        decision.intensityScale = 0.55f;
        decision.allowRender = true;
        decision.requiresEmergencyStop = false;
        break;
    case SafetyAction::RealityAnchor:
        decision.intensityScale = 0.40f;
        decision.allowRender = true;
        decision.requiresEmergencyStop = false;
        break;
    case SafetyAction::EmergencyStop:
    case SafetyAction::Lockout:
        decision.intensityScale = 0.0f;
        decision.allowRender = false;
        decision.requiresEmergencyStop = true;
        break;
    }

    decision.auditRecord = ethicalGuard.signedAuditRecord(
        "safety_governor:" + actionName(decision.action) + ":" + decision.reason,
        {decision.riskScore,
         telemetryRisk,
         cognitiveRisk,
         signalRisk,
         autonomyRisk,
         driftRisk,
         addictionRisk,
         sensoryLoad});

    return decision;
}

std::vector<float> SafetyGovernor::applyDecisionToFrame(
    const std::vector<float>& frame,
    const SafetyDecision& decision) const {

    std::vector<float> governed = frame;
    for (float& channel : governed) {
        channel = std::clamp(channel * decision.intensityScale, -1.0f, 1.0f);
    }

    return governed;
}

} // namespace dnxr
