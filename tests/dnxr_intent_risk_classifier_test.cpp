#include "../src/safety/IntentRiskClassifier.h"

#include <iostream>
#include <string>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_intent_risk_classifier_test: " << message << '\n';
        return false;
    }
    return true;
}

dnxr::IntentRiskInput nominalInput(const std::string& token) {
    dnxr::IntentRiskInput input;
    input.actionToken = token;
    input.confidence = 0.95F;
    input.ambiguityMargin = 0.20F;
    input.confirmationCount = 4;
    input.signalQuality = 0.95F;
    input.autonomyPreservation = 0.95F;
    input.cognitiveStress = 0.10F;
    input.cognitiveFatigue = 0.10F;
    input.consentActive = true;
    return input;
}

} // namespace

int main() {
    dnxr::IntentRiskClassifier classifier;

    const auto look = classifier.evaluate(nominalInput("xr.look"));
    if (!require(look.riskLevel == 0, "look should be risk level 0") ||
        !require(look.authorized, "high-confidence look should authorize") ||
        !require(look.requiredConfirmations == 1, "look should need one confirmation")) {
        return 1;
    }

    const auto interrupt = classifier.evaluate(nominalInput("xr.interrupt"));
    if (!require(interrupt.riskLevel == 4, "interrupt should be risk level 4") ||
        !require(interrupt.requiredConfirmations == 3, "interrupt should require three confirmations") ||
        !require(interrupt.authorized, "well-confirmed interrupt should authorize")) {
        return 1;
    }

    auto unconfirmed = nominalInput("xr.interrupt");
    unconfirmed.confirmationCount = 2;
    const auto hold = classifier.evaluate(unconfirmed);
    if (!require(!hold.authorized, "under-confirmed interrupt should not authorize") ||
        !require(hold.safetyMode == "intent_confirmation_hold", "under-confirmed interrupt should hold") ||
        !require(hold.reason == "intent_hold_confirmation", "under-confirmed interrupt should report confirmation hold")) {
        return 1;
    }

    auto ambiguous = nominalInput("xr.grab");
    ambiguous.ambiguityDetected = true;
    const auto ambiguityDecision = classifier.evaluate(ambiguous);
    if (!require(!ambiguityDecision.authorized, "ambiguous grab should not authorize") ||
        !require(ambiguityDecision.requiresFallback, "ambiguous grab should request fallback") ||
        !require(ambiguityDecision.safetyMode == "degraded_fallback", "ambiguous grab should degrade")) {
        return 1;
    }

    auto noConsent = nominalInput("xr.compose");
    noConsent.consentActive = false;
    const auto consentDecision = classifier.evaluate(noConsent);
    if (!require(!consentDecision.authorized, "revoked consent should block intent") ||
        !require(consentDecision.requiresRealityAnchor, "revoked consent should request reality anchor") ||
        !require(consentDecision.safetyMode == "reality_anchor_required", "revoked consent should report anchor mode")) {
        return 1;
    }

    auto unknown = nominalInput("xr.deep_modify");
    unknown.confidence = 0.94F;
    const auto unknownDecision = classifier.evaluate(unknown);
    if (!require(unknownDecision.riskLevel == 5, "unknown action should be highest risk") ||
        !require(!unknownDecision.authorized, "unknown action should not authorize below level-5 confidence") ||
        !require(unknownDecision.requiredConfidence > 0.90F, "unknown action should require high confidence")) {
        return 1;
    }

    return 0;
}
