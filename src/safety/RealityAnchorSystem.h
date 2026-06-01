#pragma once

#include "SafetyGovernor.h"

#include <string>
#include <vector>

namespace dnxr {

enum class RealityAnchorMode {
    Maintain,
    ReduceSensoryLoad,
    RealityAnchor,
    EmergencyStop,
    Lockout
};

struct RealityAnchorContext {
    std::string userAlias = "runtime_user";
    std::string sessionId = "simulation_session";
    std::string realEnvironmentLabel = "simulation_lab";
    double sessionElapsedSeconds = 0.0;
    bool consentActive = true;
    bool emergencyActive = false;
    bool postEmergencyLockout = false;
    float identityContinuity = 1.0f;
    float temporalOrientation = 1.0f;
    float realWorldOrientation = 1.0f;
    float sensoryOverload = 0.0f;
};

struct RealityAnchorPlan {
    RealityAnchorMode mode = RealityAnchorMode::Maintain;
    float riskScore = 0.0f;
    float intensityScale = 1.0f;
    bool allowRender = true;
    bool requiresEmergencyStop = false;
    bool lockoutActive = false;
    std::string reason = "anchor_nominal";
    std::string prompt;
    std::string recoveryAction = "continue_session";
    std::string traceTag;
};

class RealityAnchorSystem {
public:
    RealityAnchorPlan evaluate(
        const RealityAnchorContext& context,
        const SafetyDecision& safetyDecision) const;

    std::vector<float> applyPlanToFrame(
        const std::vector<float>& frame,
        const RealityAnchorPlan& plan) const;

    static std::string modeName(RealityAnchorMode mode);

private:
    float computeAnchorRisk(const RealityAnchorContext& context, const SafetyDecision& safetyDecision) const;
    std::string buildPrompt(const RealityAnchorContext& context, RealityAnchorMode mode) const;
};

} // namespace dnxr
