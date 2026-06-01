#include "SafetyStateMachine.h"

#include "../utils/Logger.h"

namespace dnxr {

SafetyStateMachine::SafetyStateMachine()
    : state_(SafetyRuntimeState::BootSelfTest),
      sequence_(0U) {
    auditTrail_.push_back({state_, state_, "boot_self_test_pending", sequence_, false});
}

SafetyStateTransition SafetyStateMachine::bootSelfTestPassed() {
    return transitionTo(SafetyRuntimeState::Nominal, "boot_self_test_passed", true);
}

SafetyStateTransition SafetyStateMachine::observeSafetyDecision(const SafetyDecision& decision) {
    if (decision.action == SafetyAction::Lockout) {
        return lockout(decision.reason);
    }
    if (decision.requiresEmergencyStop || decision.action == SafetyAction::EmergencyStop) {
        return latchEmergency(decision.reason);
    }

    switch (decision.action) {
    case SafetyAction::Continue:
        return transitionTo(SafetyRuntimeState::Nominal, decision.reason);
    case SafetyAction::ReduceIntensity:
        return transitionTo(SafetyRuntimeState::Degraded, decision.reason);
    case SafetyAction::Decompress:
        return transitionTo(SafetyRuntimeState::Decompression, decision.reason);
    case SafetyAction::RealityAnchor:
        return transitionTo(SafetyRuntimeState::RealityAnchor, decision.reason);
    case SafetyAction::EmergencyStop:
    case SafetyAction::Lockout:
        break;
    }
    return transitionTo(state_, "unknown_safety_action");
}

SafetyStateTransition SafetyStateMachine::observeRealityAnchorPlan(const RealityAnchorPlan& plan) {
    if (plan.lockoutActive || plan.mode == RealityAnchorMode::Lockout) {
        return lockout(plan.reason);
    }
    if (plan.requiresEmergencyStop || plan.mode == RealityAnchorMode::EmergencyStop) {
        return latchEmergency(plan.reason);
    }
    if (plan.mode == RealityAnchorMode::RealityAnchor) {
        return transitionTo(SafetyRuntimeState::RealityAnchor, plan.reason);
    }
    if (plan.mode == RealityAnchorMode::ReduceSensoryLoad) {
        return transitionTo(SafetyRuntimeState::Degraded, plan.reason);
    }
    return transitionTo(state_, plan.reason);
}

SafetyStateTransition SafetyStateMachine::latchEmergency(const std::string& reason) {
    return transitionTo(SafetyRuntimeState::EmergencyLatched, "emergency_latched:" + reason, true);
}

SafetyStateTransition SafetyStateMachine::lockout(const std::string& reason) {
    return transitionTo(SafetyRuntimeState::LockedOut, "lockout:" + reason, true);
}

SafetyStateTransition SafetyStateMachine::enterPostIncidentReview(const std::string& operatorToken) {
    if (!isExplicitOperatorToken(operatorToken)) {
        return transitionTo(state_, "post_incident_review_denied_missing_operator_token");
    }
    if (state_ != SafetyRuntimeState::EmergencyLatched && state_ != SafetyRuntimeState::LockedOut) {
        return transitionTo(state_, "post_incident_review_requires_latched_state");
    }
    return transitionTo(SafetyRuntimeState::PostIncidentReview, "operator_entered_post_incident_review", true);
}

SafetyStateTransition SafetyStateMachine::resetToNominalAfterReview(const std::string& operatorToken) {
    if (!isExplicitOperatorToken(operatorToken)) {
        return transitionTo(state_, "reset_denied_missing_operator_token");
    }
    if (state_ != SafetyRuntimeState::PostIncidentReview) {
        return transitionTo(state_, "reset_requires_post_incident_review");
    }
    return transitionTo(SafetyRuntimeState::Nominal, "operator_reset_after_post_incident_review", true);
}

SafetyRuntimeState SafetyStateMachine::state() const {
    return state_;
}

bool SafetyStateMachine::isEmergencyLatched() const {
    return state_ == SafetyRuntimeState::EmergencyLatched;
}

bool SafetyStateMachine::isLockedOut() const {
    return state_ == SafetyRuntimeState::LockedOut;
}

std::vector<SafetyStateTransition> SafetyStateMachine::auditTrail() const {
    return auditTrail_;
}

std::string SafetyStateMachine::toString(SafetyRuntimeState state) {
    switch (state) {
    case SafetyRuntimeState::BootSelfTest: return "boot_self_test";
    case SafetyRuntimeState::Nominal: return "nominal";
    case SafetyRuntimeState::Degraded: return "degraded";
    case SafetyRuntimeState::Decompression: return "decompression";
    case SafetyRuntimeState::RealityAnchor: return "reality_anchor";
    case SafetyRuntimeState::EmergencyLatched: return "emergency_latched";
    case SafetyRuntimeState::LockedOut: return "locked_out";
    case SafetyRuntimeState::PostIncidentReview: return "post_incident_review";
    }
    return "unknown";
}

SafetyStateTransition SafetyStateMachine::transitionTo(
    SafetyRuntimeState next,
    const std::string& reason,
    bool explicitOverride) {

    const SafetyRuntimeState previous = state_;
    if ((state_ == SafetyRuntimeState::EmergencyLatched || state_ == SafetyRuntimeState::LockedOut) && !explicitOverride) {
        next = state_;
    }

    state_ = next;
    SafetyStateTransition transition{previous, state_, reason, ++sequence_, isEmergencyLatched() || isLockedOut()};
    auditTrail_.push_back(transition);

    Logger::info("[SafetyStateMachine] " + toString(previous) + " -> " + toString(state_) +
                 " reason=" + reason + " seq=" + std::to_string(sequence_));
    return transition;
}

bool SafetyStateMachine::isExplicitOperatorToken(const std::string& operatorToken) {
    return operatorToken.rfind("operator:", 0) == 0 && operatorToken.size() > 9U;
}

} // namespace dnxr
