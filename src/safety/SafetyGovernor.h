#pragma once

#include <string>
#include <vector>

#include "SafetyDecision.h"
#include "../neurotech/DeepImmersionPlatform.h"

namespace dnxr {

struct SafetyGovernorInput {
    std::vector<float> normalizedTelemetryRisk;
    std::vector<float> sensoryFrame;
    std::string inferredState;
    CognitiveState cognitive;
    BiometricFrame biometrics;
    NeuroPipelineDiagnostics diagnostics;
};

class SafetyGovernor {
public:
    SafetyDecision evaluate(const SafetyGovernorInput& input) const;
    std::vector<float> applyDecisionToFrame(
        const std::vector<float>& frame,
        const SafetyDecision& decision) const;

private:
    float computeTelemetryRisk(const std::vector<float>& normalizedTelemetryRisk) const;
    float computeSensoryLoad(const std::vector<float>& sensoryFrame) const;
    std::string actionName(SafetyAction action) const;
};

} // namespace dnxr
