#pragma once

#include <string>
#include <vector>

#include "../neurotech/DeepImmersionPlatform.h"

namespace dnxr {

enum class SafetyAction {
    Continue,
    ReduceIntensity,
    Decompress,
    RealityAnchor,
    EmergencyStop,
    Lockout
};

struct SafetyGovernorInput {
    std::vector<float> normalizedTelemetryRisk;
    std::vector<float> sensoryFrame;
    std::string inferredState;
    CognitiveState cognitive;
    BiometricFrame biometrics;
    NeuroPipelineDiagnostics diagnostics;
};

struct SafetyDecision {
    SafetyAction action = SafetyAction::Continue;
    float riskScore = 0.0f;
    float intensityScale = 1.0f;
    bool allowRender = true;
    bool requiresEmergencyStop = false;
    std::string reason;
    std::string auditRecord;
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
