#pragma once

#include "RealityAnchorSystem.h"
#include "SafetyGovernor.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace dnxr {

enum class SafetyRuntimeState {
    Booting,
    EvidenceCheck,
    Connecting,
    Calibrating,
    Ready,
    Running,
    AwaitingAck,
    HoldNeutral,
    HoldMinimal,
    Recalibrating,
    Decompressing,
    ControlledResume,
    EmergencyLatched,
    Shutdown,
    LockedOut,
    PostIncidentReview,
    BootSelfTest = Booting,
    Nominal = Running,
    Degraded = HoldNeutral,
    Decompression = Decompressing,
    RealityAnchor = HoldMinimal
};

struct SafetyStateTransition {
    SafetyRuntimeState from = SafetyRuntimeState::BootSelfTest;
    SafetyRuntimeState to = SafetyRuntimeState::BootSelfTest;
    std::string reason = "initialized";
    std::uint64_t sequence = 0U;
    bool latched = false;
    std::string authority = "canonical_safety_state_machine";
    std::uint64_t timestampNs = 0U;
};

class SafetyStateMachine {
public:
    SafetyStateMachine();

    SafetyStateTransition bootSelfTestPassed();
    SafetyStateTransition evidenceAccepted(const std::string& authority);
    SafetyStateTransition connected(const std::string& authority);
    SafetyStateTransition calibrated(const std::string& authority);
    SafetyStateTransition startRunning(const std::string& authority);
    SafetyStateTransition awaitAck(const std::string& authority);
    SafetyStateTransition enterControlledResume(const std::string& authority);
    SafetyStateTransition shutdown(const std::string& authority);
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
    SafetyStateTransition transitionToLocked(SafetyRuntimeState next, const std::string& reason, bool explicitOverride = false);
    SafetyStateTransition latchEmergencyLocked(const std::string& reason);
    SafetyStateTransition lockoutLocked(const std::string& reason);
    static bool isExplicitOperatorToken(const std::string& operatorToken);

    mutable std::mutex mutex_;
    SafetyRuntimeState state_;
    std::uint64_t sequence_;
    std::vector<SafetyStateTransition> auditTrail_;
};

} // namespace dnxr
