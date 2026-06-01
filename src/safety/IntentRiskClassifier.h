#pragma once

#include <string>

namespace dnxr {

struct IntentRiskInput {
    std::string actionToken = "none";
    float confidence = 0.0f;
    float ambiguityMargin = 0.0f;
    bool ambiguityDetected = false;
    bool fallbackRecommended = false;
    int confirmationCount = 0;
    float signalQuality = 1.0f;
    float autonomyPreservation = 1.0f;
    float cognitiveStress = 0.0f;
    float cognitiveFatigue = 0.0f;
    bool consentActive = true;
};

struct IntentRiskDecision {
    int riskLevel = 0;
    int requiredConfirmations = 1;
    float requiredConfidence = 0.45f;
    bool authorized = false;
    bool requiresFallback = false;
    bool requiresRealityAnchor = false;
    std::string reason = "intent_none";
    std::string safetyMode = "nominal";
};

class IntentRiskClassifier {
public:
    IntentRiskDecision evaluate(const IntentRiskInput& input) const;

    static int riskLevelForToken(const std::string& actionToken);
    static std::string riskName(int riskLevel);

private:
    float normalizedCognitiveLoad(const IntentRiskInput& input) const;
};

} // namespace dnxr
