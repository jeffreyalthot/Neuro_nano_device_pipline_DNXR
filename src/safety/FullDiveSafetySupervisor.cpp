#include "FullDiveSafetySupervisor.h"

namespace dnxr {

FullDiveSafetyDecision FullDiveSafetySupervisor::evaluate(const FullDiveNeuralFrame& frame) const {
    FullDiveSafetyDecision d;
    if (!frame.interoceptiveSafety.patientStopSignalAlive) {
        d.reason = "patient_stop_channel_lost";
        d.requiresEmergencyStop = true;
        d.auditCode = "FDSS_STOP_CHANNEL_LOST";
        return d;
    }
    if (!frame.safety.painSimulationBlocked) {
        d.reason = "pain_or_forbidden_perception_requested";
        d.requiresEmergencyStop = true;
        d.auditCode = "FDSS_NO_PAIN_VIOLATION";
        return d;
    }
    if (!frame.safety.emergencyCutoffAvailable) {
        d.reason = "hardware_cutoff_unverified";
        d.requiresEmergencyStop = true;
        d.auditCode = "FDSS_CUTOFF_MISSING";
        return d;
    }
    if (frame.safety.aggregateRisk >= 0.80f) {
        d.reason = "aggregate_risk_requires_reality_anchor";
        d.action = FullDiveSafetyAction::RealityAnchor;
        d.severity = "high";
        d.maxAllowedIntensityScale = 0.25f;
        d.requiresEmergencyStop = false;
        d.auditCode = "FDSS_RISK_ANCHOR";
        return d;
    }
    d.allowed = true;
    d.action = FullDiveSafetyAction::Allow;
    d.reason = "full_dive_safety_supervisor_allow";
    d.severity = "nominal";
    d.maxAllowedIntensityScale = 1.0f;
    d.requiresRealityAnchor = frame.realityAnchor.anchorRequired;
    d.auditCode = "FDSS_ALLOW";
    return d;
}

} // namespace dnxr
