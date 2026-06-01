#include "RealityAnchorSystem.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float finiteOrDefault(float value, float fallback) {
    return std::isfinite(value) ? value : fallback;
}

float actionFloorRisk(SafetyAction action) {
    switch (action) {
        case SafetyAction::Continue:
            return 0.0f;
        case SafetyAction::ReduceIntensity:
            return 0.45f;
        case SafetyAction::Decompress:
            return 0.55f;
        case SafetyAction::RealityAnchor:
            return 0.70f;
        case SafetyAction::EmergencyStop:
            return 0.95f;
        case SafetyAction::Lockout:
            return 1.0f;
    }
    return 0.0f;
}

} // namespace

std::string RealityAnchorSystem::modeName(RealityAnchorMode mode) {
    switch (mode) {
        case RealityAnchorMode::Maintain:
            return "maintain";
        case RealityAnchorMode::ReduceSensoryLoad:
            return "reduce_sensory_load";
        case RealityAnchorMode::RealityAnchor:
            return "reality_anchor";
        case RealityAnchorMode::EmergencyStop:
            return "emergency_stop";
        case RealityAnchorMode::Lockout:
            return "lockout";
    }
    return "unknown";
}

RealityAnchorPlan RealityAnchorSystem::evaluate(
    const RealityAnchorContext& context,
    const SafetyDecision& safetyDecision) const {

    RealityAnchorPlan plan;
    plan.riskScore = computeAnchorRisk(context, safetyDecision);

    if (context.postEmergencyLockout || safetyDecision.action == SafetyAction::Lockout) {
        plan.mode = RealityAnchorMode::Lockout;
        plan.intensityScale = 0.0f;
        plan.allowRender = false;
        plan.requiresEmergencyStop = true;
        plan.lockoutActive = true;
        plan.reason = "post_emergency_lockout";
        plan.recoveryAction = "supervised_lockout_review";
    } else if (!context.consentActive) {
        plan.mode = RealityAnchorMode::EmergencyStop;
        plan.intensityScale = 0.0f;
        plan.allowRender = false;
        plan.requiresEmergencyStop = true;
        plan.reason = "consent_inactive";
        plan.recoveryAction = "restore_consent_before_resume";
    } else if (context.emergencyActive || safetyDecision.requiresEmergencyStop ||
               safetyDecision.action == SafetyAction::EmergencyStop) {
        plan.mode = RealityAnchorMode::EmergencyStop;
        plan.intensityScale = 0.0f;
        plan.allowRender = false;
        plan.requiresEmergencyStop = true;
        plan.reason = "safety_emergency_stop";
        plan.recoveryAction = "emergency_stop_and_reorient";
    } else if (safetyDecision.action == SafetyAction::RealityAnchor || plan.riskScore >= 0.65f) {
        plan.mode = RealityAnchorMode::RealityAnchor;
        plan.intensityScale = std::min(clamp01(safetyDecision.intensityScale), 0.35f);
        plan.allowRender = safetyDecision.allowRender;
        plan.reason = "identity_or_orientation_anchor_required";
        plan.recoveryAction = "reality_anchor_sequence";
    } else if (safetyDecision.action == SafetyAction::Decompress ||
               safetyDecision.action == SafetyAction::ReduceIntensity ||
               plan.riskScore >= 0.45f) {
        plan.mode = RealityAnchorMode::ReduceSensoryLoad;
        plan.intensityScale = std::min(clamp01(safetyDecision.intensityScale), 0.55f);
        plan.allowRender = safetyDecision.allowRender;
        plan.reason = "sensory_decompression_required";
        plan.recoveryAction = "gradual_sensory_reduction";
    } else {
        plan.mode = RealityAnchorMode::Maintain;
        plan.intensityScale = clamp01(safetyDecision.intensityScale);
        plan.allowRender = safetyDecision.allowRender;
        plan.reason = "anchor_nominal";
        plan.recoveryAction = "continue_session";
    }

    plan.prompt = buildPrompt(context, plan.mode);
    plan.traceTag = "reality_anchor:" + modeName(plan.mode) + ":" + plan.reason;
    return plan;
}

std::vector<float> RealityAnchorSystem::applyPlanToFrame(
    const std::vector<float>& frame,
    const RealityAnchorPlan& plan) const {

    if (!plan.allowRender || plan.requiresEmergencyStop || plan.lockoutActive) {
        return std::vector<float>(frame.size(), 0.0f);
    }

    const float scale = clamp01(plan.intensityScale);
    std::vector<float> anchored = frame;
    for (float& value : anchored) {
        const float finiteValue = finiteOrDefault(value, 0.0f);
        value = std::clamp(finiteValue * scale, -scale, scale);
    }

    if (plan.mode == RealityAnchorMode::RealityAnchor && !anchored.empty()) {
        anchored.front() = std::clamp(anchored.front() * 0.75f, -0.25f, 0.25f);
    }

    return anchored;
}

float RealityAnchorSystem::computeAnchorRisk(
    const RealityAnchorContext& context,
    const SafetyDecision& safetyDecision) const {

    const float identityRisk = 1.0f - clamp01(finiteOrDefault(context.identityContinuity, 1.0f));
    const float temporalRisk = 1.0f - clamp01(finiteOrDefault(context.temporalOrientation, 1.0f));
    const float environmentRisk = 1.0f - clamp01(finiteOrDefault(context.realWorldOrientation, 1.0f));
    const float overloadRisk = clamp01(finiteOrDefault(context.sensoryOverload, 0.0f));
    const float safetyRisk = std::max(
        clamp01(finiteOrDefault(safetyDecision.riskScore, 0.0f)),
        actionFloorRisk(safetyDecision.action));

    return clamp01(
        (identityRisk * 0.25f) +
        (temporalRisk * 0.20f) +
        (environmentRisk * 0.20f) +
        (overloadRisk * 0.15f) +
        (safetyRisk * 0.20f));
}

std::string RealityAnchorSystem::buildPrompt(
    const RealityAnchorContext& context,
    RealityAnchorMode mode) const {

    std::ostringstream prompt;
    prompt << "user=" << context.userAlias
           << "|session=" << context.sessionId
           << "|environment=" << context.realEnvironmentLabel
           << "|elapsed_s=" << static_cast<long long>(std::max(0.0, context.sessionElapsedSeconds))
           << "|mode=" << modeName(mode);
    return prompt.str();
}

} // namespace dnxr
