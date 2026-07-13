#include "SafetyStateMachine.h"

#include "../utils/Logger.h"

namespace dnxr {

SafetyStateMachine::SafetyStateMachine()
    : state_(SafetyRuntimeState::BootSelfTest),
      sequence_(0U) {
    auditTrail_.push_back({state_, state_, "booting_evidence_check_pending", sequence_, false, "canonical_safety_state_machine", 0U});
}

SafetyStateTransition SafetyStateMachine::bootSelfTestPassed() {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::EvidenceCheck, "boot_self_test_passed", true);
}


SafetyStateTransition SafetyStateMachine::evidenceAccepted(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::Connecting, "evidence_accepted_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::connected(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::Calibrating, "transport_connected_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::calibrated(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::Ready, "calibrated_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::startRunning(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::Running, "controlled_start_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::awaitAck(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::AwaitingAck, "awaiting_ack_by:" + authority);
}

SafetyStateTransition SafetyStateMachine::enterControlledResume(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::ControlledResume, "controlled_resume_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::shutdown(const std::string& authority) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transitionToLocked(SafetyRuntimeState::Shutdown, "shutdown_by:" + authority, true);
}

SafetyStateTransition SafetyStateMachine::observeSafetyDecision(const SafetyDecision& decision) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (decision.action == SafetyAction::Lockout) {
        return lockoutLocked(decision.reason);
    }
    if (decision.requiresEmergencyStop || decision.action == SafetyAction::EmergencyStop) {
        return latchEmergencyLocked(decision.reason);
    }

    switch (decision.action) {
    case SafetyAction::Continue:
        return transitionToLocked(SafetyRuntimeState::Running, decision.reason);
    case SafetyAction::ReduceIntensity:
        return transitionToLocked(SafetyRuntimeState::HoldNeutral, decision.reason);
    case SafetyAction::Decompress:
        return transitionToLocked(SafetyRuntimeState::Decompressing, decision.reason);
    case SafetyAction::RealityAnchor:
        return transitionToLocked(SafetyRuntimeState::HoldMinimal, decision.reason);
    case SafetyAction::EmergencyStop:
    case SafetyAction::Lockout:
        break;
    }
    return transitionToLocked(state_, "unknown_safety_action");
}

SafetyStateTransition SafetyStateMachine::observeRealityAnchorPlan(const RealityAnchorPlan& plan) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (plan.lockoutActive || plan.mode == RealityAnchorMode::Lockout) {
        return lockoutLocked(plan.reason);
    }
    if (plan.requiresEmergencyStop || plan.mode == RealityAnchorMode::EmergencyStop) {
        return latchEmergencyLocked(plan.reason);
    }
    if (plan.mode == RealityAnchorMode::RealityAnchor) {
        return transitionToLocked(SafetyRuntimeState::HoldMinimal, plan.reason);
    }
    if (plan.mode == RealityAnchorMode::ReduceSensoryLoad) {
        return transitionToLocked(SafetyRuntimeState::HoldNeutral, plan.reason);
    }
    return transitionToLocked(state_, plan.reason);
}

SafetyStateTransition SafetyStateMachine::latchEmergency(const std::string& reason) {
    std::lock_guard<std::mutex> lock(mutex_);
    return latchEmergencyLocked(reason);
}

SafetyStateTransition SafetyStateMachine::lockout(const std::string& reason) {
    std::lock_guard<std::mutex> lock(mutex_);
    return lockoutLocked(reason);
}

SafetyStateTransition SafetyStateMachine::latchEmergencyLocked(const std::string& reason) {
    return transitionToLocked(SafetyRuntimeState::EmergencyLatched, "emergency_latched:" + reason, true);
}

SafetyStateTransition SafetyStateMachine::lockoutLocked(const std::string& reason) {
    return transitionToLocked(SafetyRuntimeState::LockedOut, "lockout:" + reason, true);
}

SafetyStateTransition SafetyStateMachine::enterPostIncidentReview(const std::string& operatorToken) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isExplicitOperatorToken(operatorToken)) {
        return transitionToLocked(state_, "post_incident_review_denied_missing_operator_token");
    }
    if (state_ != SafetyRuntimeState::EmergencyLatched && state_ != SafetyRuntimeState::LockedOut) {
        return transitionToLocked(state_, "post_incident_review_requires_latched_state");
    }
    return transitionToLocked(SafetyRuntimeState::PostIncidentReview, "operator_entered_post_incident_review", true);
}

SafetyStateTransition SafetyStateMachine::resetToNominalAfterReview(const std::string& operatorToken) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isExplicitOperatorToken(operatorToken)) {
        return transitionToLocked(state_, "reset_denied_missing_operator_token");
    }
    if (state_ != SafetyRuntimeState::PostIncidentReview) {
        return transitionToLocked(state_, "reset_requires_post_incident_review");
    }
    return transitionToLocked(SafetyRuntimeState::ControlledResume, "operator_reset_after_post_incident_review_requires_controlled_resume", true);
}

SafetyRuntimeState SafetyStateMachine::state() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

bool SafetyStateMachine::isEmergencyLatched() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_ == SafetyRuntimeState::EmergencyLatched;
}

bool SafetyStateMachine::isLockedOut() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_ == SafetyRuntimeState::LockedOut;
}

std::vector<SafetyStateTransition> SafetyStateMachine::auditTrail() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return auditTrail_;
}

std::string SafetyStateMachine::toString(SafetyRuntimeState state) {
    switch (state) {
    case SafetyRuntimeState::Booting: return "booting";
    case SafetyRuntimeState::EvidenceCheck: return "evidence_check";
    case SafetyRuntimeState::Connecting: return "connecting";
    case SafetyRuntimeState::Calibrating: return "calibrating";
    case SafetyRuntimeState::Ready: return "ready";
    case SafetyRuntimeState::Running: return "running";
    case SafetyRuntimeState::AwaitingAck: return "awaiting_ack";
    case SafetyRuntimeState::HoldNeutral: return "hold_neutral";
    case SafetyRuntimeState::HoldMinimal: return "hold_minimal";
    case SafetyRuntimeState::Recalibrating: return "recalibrating";
    case SafetyRuntimeState::Decompressing: return "decompressing";
    case SafetyRuntimeState::ControlledResume: return "controlled_resume";
    case SafetyRuntimeState::EmergencyLatched: return "emergency_latched";
    case SafetyRuntimeState::LockedOut: return "locked_out";
    case SafetyRuntimeState::Shutdown: return "shutdown";
    case SafetyRuntimeState::PostIncidentReview: return "post_incident_review";
    }
    return "unknown";
}

SafetyStateTransition SafetyStateMachine::transitionToLocked(
    SafetyRuntimeState next,
    const std::string& reason,
    bool explicitOverride) {

    const SafetyRuntimeState previous = state_;
    if ((state_ == SafetyRuntimeState::EmergencyLatched || state_ == SafetyRuntimeState::LockedOut) && !explicitOverride) {
        next = state_;
    }

    state_ = next;
    SafetyStateTransition transition{previous, state_, reason, ++sequence_, state_ == SafetyRuntimeState::EmergencyLatched || state_ == SafetyRuntimeState::LockedOut, "canonical_safety_state_machine", sequence_};
    auditTrail_.push_back(transition);

    Logger::info("[SafetyStateMachine] " + toString(previous) + " -> " + toString(state_) +
                 " reason=" + reason + " seq=" + std::to_string(sequence_));
    return transition;
}

bool SafetyStateMachine::isExplicitOperatorToken(const std::string& operatorToken) {
    return operatorToken.rfind("operator:", 0) == 0 && operatorToken.size() > 9U;
}

} // namespace dnxr
