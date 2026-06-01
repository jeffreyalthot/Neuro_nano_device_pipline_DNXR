#include "IntentRiskClassifier.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float finiteOrDefault(float value, float fallback) {
    return std::isfinite(value) ? value : fallback;
}

int confirmationRequirementForRisk(int riskLevel) {
    if (riskLevel >= 5) {
        return 4;
    }
    if (riskLevel >= 4) {
        return 3;
    }
    if (riskLevel >= 3) {
        return 2;
    }
    return 1;
}

float confidenceRequirementForRisk(int riskLevel) {
    if (riskLevel >= 5) {
        return 0.95f;
    }
    if (riskLevel >= 4) {
        return 0.88f;
    }
    if (riskLevel >= 3) {
        return 0.72f;
    }
    if (riskLevel >= 2) {
        return 0.58f;
    }
    return 0.45f;
}

} // namespace

IntentRiskDecision IntentRiskClassifier::evaluate(const IntentRiskInput& input) const {
    IntentRiskDecision decision;
    decision.riskLevel = riskLevelForToken(input.actionToken);
    decision.requiredConfirmations = confirmationRequirementForRisk(decision.riskLevel);
    decision.requiredConfidence = confidenceRequirementForRisk(decision.riskLevel);

    const float confidence = clamp01(finiteOrDefault(input.confidence, 0.0f));
    const float ambiguityMargin = clamp01(finiteOrDefault(input.ambiguityMargin, 0.0f));
    const float signalQuality = clamp01(finiteOrDefault(input.signalQuality, 0.0f));
    const float autonomy = clamp01(finiteOrDefault(input.autonomyPreservation, 0.0f));
    const float cognitiveLoad = normalizedCognitiveLoad(input);

    if (input.actionToken == "none") {
        decision.reason = "intent_none";
        decision.safetyMode = "nominal";
        return decision;
    }

    if (!input.consentActive || autonomy < 0.35f) {
        decision.reason = "intent_blocked_autonomy_or_consent";
        decision.safetyMode = "reality_anchor_required";
        decision.requiresFallback = true;
        decision.requiresRealityAnchor = true;
        return decision;
    }

    if (input.ambiguityDetected || input.fallbackRecommended || ambiguityMargin < 0.05f) {
        decision.reason = "intent_blocked_ambiguity";
        decision.safetyMode = "degraded_fallback";
        decision.requiresFallback = true;
        return decision;
    }

    if (signalQuality < 0.45f) {
        decision.reason = "intent_blocked_low_signal_quality";
        decision.safetyMode = "degraded_fallback";
        decision.requiresFallback = true;
        return decision;
    }

    if (cognitiveLoad > 0.85f && decision.riskLevel >= 3) {
        decision.reason = "intent_blocked_cognitive_load";
        decision.safetyMode = "intent_confirmation_hold";
        decision.requiresFallback = true;
        return decision;
    }

    if (confidence < decision.requiredConfidence) {
        decision.reason = "intent_hold_confidence";
        decision.safetyMode = "intent_confirmation_hold";
        decision.requiresFallback = true;
        return decision;
    }

    if (input.confirmationCount < decision.requiredConfirmations) {
        decision.reason = "intent_hold_confirmation";
        decision.safetyMode = "intent_confirmation_hold";
        decision.requiresFallback = true;
        return decision;
    }

    decision.authorized = true;
    decision.reason = "intent_authorized_risk_" + std::to_string(decision.riskLevel);
    decision.safetyMode = "nominal";
    return decision;
}

int IntentRiskClassifier::riskLevelForToken(const std::string& actionToken) {
    if (actionToken == "none") {
        return 0;
    }
    if (actionToken == "xr.look") {
        return 0;
    }
    if (actionToken == "xr.move" || actionToken == "xr.rotate") {
        return 1;
    }
    if (actionToken == "xr.grab") {
        return 2;
    }
    if (actionToken == "xr.compose") {
        return 3;
    }
    if (actionToken == "xr.cancel" || actionToken == "xr.interrupt") {
        return 4;
    }
    return 5;
}

std::string IntentRiskClassifier::riskName(int riskLevel) {
    switch (riskLevel) {
        case 0:
            return "observe";
        case 1:
            return "locomotion_or_view";
        case 2:
            return "manipulation";
        case 3:
            return "composed_interaction";
        case 4:
            return "system_critical";
        default:
            return "cognitive_or_unknown";
    }
}

float IntentRiskClassifier::normalizedCognitiveLoad(const IntentRiskInput& input) const {
    const float stress = clamp01(finiteOrDefault(input.cognitiveStress, 0.0f));
    const float fatigue = clamp01(finiteOrDefault(input.cognitiveFatigue, 0.0f));
    return clamp01((stress * 0.55f) + (fatigue * 0.45f));
}

} // namespace dnxr
