#pragma once

#include "RealityAnchorSystem.h"
#include "SafetyGovernor.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace dnxr {

enum class SafetyRuntimeState {
    BootSelfTest,
    Nominal,
    Degraded,
    Decompression,
    RealityAnchor,
    EmergencyLatched,
    LockedOut,
    PostIncidentReview
};

struct SafetyStateTransition {
    SafetyRuntimeState from = SafetyRuntimeState::BootSelfTest;
    SafetyRuntimeState to = SafetyRuntimeState::BootSelfTest;
    std::string reason = "initialized";
    std::uint64_t sequence = 0U;
    bool latched = false;
};

class SafetyStateMachine {
public:
    SafetyStateMachine();

    SafetyStateTransition bootSelfTestPassed();
    SafetyStateTransition observeSafetyDecision(const SafetyDecision& decision);
    SafetyStateTransition observeRealityAnchorPlan(const RealityAnchorPlan& plan);
    SafetyStateTransition latchEmergency(const std::string& reason);
    SafetyStateTransition lockout(const std::string& reason);
    SafetyStateTransition enterPostIncidentReview(const std::string& operatorToken);
    SafetyStateTransition resetToNominalAfterReview(const std::string& operatorToken);

    SafetyRuntimeState state() const;
    bool isEmergencyLatched() const;
    bool isLockedOut() const;
    std::vector<SafetyStateTransition> auditTrail() const;

    static std::string toString(SafetyRuntimeState state);

private:
    SafetyStateTransition transitionTo(SafetyRuntimeState next, const std::string& reason, bool explicitOverride = false);
    static bool isExplicitOperatorToken(const std::string& operatorToken);

    mutable std::recursive_mutex mutex_;
    SafetyRuntimeState state_;
    std::uint64_t sequence_;
    std::vector<SafetyStateTransition> auditTrail_;
};

} // namespace dnxr
