#include "../src/safety/RealityAnchorSystem.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_reality_anchor_system_test: " << message << '\n';
        return false;
    }
    return true;
}

dnxr::RealityAnchorContext nominalContext() {
    dnxr::RealityAnchorContext context;
    context.userAlias = "sim_user";
    context.sessionId = "session_001";
    context.realEnvironmentLabel = "simulation_lab";
    context.sessionElapsedSeconds = 42.0;
    context.identityContinuity = 0.98F;
    context.temporalOrientation = 0.97F;
    context.realWorldOrientation = 0.96F;
    context.sensoryOverload = 0.10F;
    return context;
}

dnxr::SafetyDecision nominalDecision() {
    dnxr::SafetyDecision decision;
    decision.action = dnxr::SafetyAction::Continue;
    decision.riskScore = 0.10F;
    decision.intensityScale = 1.0F;
    decision.allowRender = true;
    decision.requiresEmergencyStop = false;
    decision.reason = "nominal";
    return decision;
}

} // namespace

int main() {
    dnxr::RealityAnchorSystem anchor;

    const auto nominalPlan = anchor.evaluate(nominalContext(), nominalDecision());
    if (!require(nominalPlan.mode == dnxr::RealityAnchorMode::Maintain, "nominal context should maintain immersion") ||
        !require(nominalPlan.allowRender, "nominal plan should allow rendering") ||
        !require(!nominalPlan.requiresEmergencyStop, "nominal plan should not trigger emergency stop") ||
        !require(nominalPlan.prompt.find("session_001") != std::string::npos, "prompt should include the session id")) {
        return 1;
    }

    auto anchorDecision = nominalDecision();
    anchorDecision.action = dnxr::SafetyAction::RealityAnchor;
    anchorDecision.riskScore = 0.80F;
    anchorDecision.intensityScale = 0.40F;
    auto anchorContext = nominalContext();
    anchorContext.identityContinuity = 0.40F;
    anchorContext.realWorldOrientation = 0.45F;
    const auto anchorPlan = anchor.evaluate(anchorContext, anchorDecision);
    if (!require(anchorPlan.mode == dnxr::RealityAnchorMode::RealityAnchor, "identity drift should trigger reality anchor") ||
        !require(anchorPlan.intensityScale <= 0.35F, "reality anchor should cap sensory intensity") ||
        !require(anchorPlan.recoveryAction == "reality_anchor_sequence", "reality anchor should request anchor recovery")) {
        return 1;
    }

    const std::vector<float> frame{1.0F, -0.8F, 0.5F, std::nanf("")};
    const auto anchoredFrame = anchor.applyPlanToFrame(frame, anchorPlan);
    if (!require(anchoredFrame.size() == frame.size(), "anchored frame should preserve channel count") ||
        !require(std::abs(anchoredFrame[0]) <= 0.25F, "anchor channel should be strongly bounded") ||
        !require(std::abs(anchoredFrame[1]) <= anchorPlan.intensityScale, "channels should respect intensity scale") ||
        !require(anchoredFrame[3] == 0.0F, "non-finite channel values should be neutralized")) {
        return 1;
    }

    auto consentContext = nominalContext();
    consentContext.consentActive = false;
    const auto consentPlan = anchor.evaluate(consentContext, nominalDecision());
    if (!require(consentPlan.mode == dnxr::RealityAnchorMode::EmergencyStop, "revoked consent should emergency stop") ||
        !require(!consentPlan.allowRender, "revoked consent should disable render") ||
        !require(consentPlan.requiresEmergencyStop, "revoked consent should require emergency stop")) {
        return 1;
    }

    auto lockoutContext = nominalContext();
    lockoutContext.postEmergencyLockout = true;
    const auto lockoutPlan = anchor.evaluate(lockoutContext, nominalDecision());
    if (!require(lockoutPlan.mode == dnxr::RealityAnchorMode::Lockout, "post-emergency state should lock out") ||
        !require(lockoutPlan.lockoutActive, "lockout plan should mark lockout active") ||
        !require(anchor.applyPlanToFrame(frame, lockoutPlan) == std::vector<float>(frame.size(), 0.0F),
                 "lockout should zero the sensory frame")) {
        return 1;
    }

    return 0;
}
