#include "FullDiveSafetyBridge.h"

#include <algorithm>

namespace dnxr {
namespace {

SafetyAction toSafetyAction(FullDiveInterventionLevel level) {
    switch (level) {
    case FullDiveInterventionLevel::None: return SafetyAction::Continue;
    case FullDiveInterventionLevel::ReduceIntensity: return SafetyAction::ReduceIntensity;
    case FullDiveInterventionLevel::Decompress: return SafetyAction::Decompress;
    case FullDiveInterventionLevel::RealityAnchor: return SafetyAction::RealityAnchor;
    case FullDiveInterventionLevel::EmergencyStop: return SafetyAction::EmergencyStop;
    case FullDiveInterventionLevel::Lockout: return SafetyAction::Lockout;
    }
    return SafetyAction::EmergencyStop;
}

bool isMoreSevere(FullDiveInterventionLevel candidate, FullDiveInterventionLevel current) {
    return static_cast<int>(candidate) > static_cast<int>(current);
}

void raise(
    FullDiveRuntimeSnapshot& snapshot,
    FullDiveInterventionLevel level,
    float severity,
    const std::string& reason,
    const std::string& traceTag) {

    if (isMoreSevere(level, snapshot.intervention) || severity > snapshot.severity) {
        snapshot.intervention = level;
        snapshot.severity = std::clamp(severity, 0.0f, 1.0f);
        snapshot.reason = reason;
        snapshot.traceTag = traceTag;
    }
}

} // namespace

FullDiveRuntimeSnapshot FullDiveSafetyBridge::evaluate(const FullDiveSynchronizationEngine& engine) const {
    FullDiveRuntimeSnapshot snapshot;
    snapshot.state = engine.getState();

    if (snapshot.state.emergencyExitTriggered) {
        raise(snapshot, FullDiveInterventionLevel::EmergencyStop, 1.0f,
              "full_dive_emergency_exit", "full_dive:emergency_exit");
    }

    if (snapshot.state.neuralOverloadDetected) {
        const float severity = std::clamp(1.0f - snapshot.state.fullDiveStability, 0.0f, 1.0f);
        raise(snapshot,
              severity > 0.80f ? FullDiveInterventionLevel::EmergencyStop : FullDiveInterventionLevel::Decompress,
              severity,
              "full_dive_neural_overload",
              "full_dive:neural_overload");
    } else if (snapshot.state.fullDiveStability < 0.55f) {
        raise(snapshot, FullDiveInterventionLevel::ReduceIntensity, 0.55f - snapshot.state.fullDiveStability,
              "full_dive_stability_margin_low", "full_dive:reduce_intensity");
    }

    if (snapshot.state.consciousnessDesynchronization) {
        const float temporalSeverity = std::clamp(1.0f - snapshot.state.temporalCoherence, 0.0f, 1.0f);
        raise(snapshot,
              temporalSeverity > 0.75f ? FullDiveInterventionLevel::EmergencyStop : FullDiveInterventionLevel::RealityAnchor,
              temporalSeverity,
              "full_dive_consciousness_desynchronization",
              "full_dive:reality_anchor");
    }

    if (snapshot.state.temporalCoherence < 0.20f && snapshot.state.vestibularStability < 0.20f) {
        raise(snapshot, FullDiveInterventionLevel::Lockout, 1.0f,
              "full_dive_repeated_anchor_failure_lockout", "full_dive:lockout");
    }

    return snapshot;
}

void FullDiveSafetyBridge::apply(const FullDiveRuntimeSnapshot& snapshot, SafetyStateMachine& stateMachine) const {
    if (!snapshot.safetyInterventionRequired()) {
        return;
    }
    if (snapshot.intervention == FullDiveInterventionLevel::Lockout) {
        stateMachine.lockout(snapshot.reason);
        return;
    }
    if (snapshot.intervention == FullDiveInterventionLevel::EmergencyStop) {
        stateMachine.latchEmergency(snapshot.reason);
        return;
    }

    SafetyDecision decision;
    decision.action = toSafetyAction(snapshot.intervention);
    decision.reason = snapshot.reason;
    decision.riskScore = snapshot.severity;
    decision.intensityScale = std::clamp(1.0f - snapshot.severity, 0.0f, 1.0f);
    decision.allowRender = snapshot.intervention != FullDiveInterventionLevel::EmergencyStop &&
                           snapshot.intervention != FullDiveInterventionLevel::Lockout;
    decision.requiresEmergencyStop = false;
    stateMachine.observeSafetyDecision(decision);
}

} // namespace dnxr
