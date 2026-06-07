#include "FullDiveSafetySupervisor.h"

namespace dnxr {

FullDiveSafetyDecision FullDiveSafetySupervisor::evaluate(const FullDiveNeuralFrame& frame) const {
    FullDiveSafetyDecision d;
    const auto emergency = [&](const std::string& reason, const std::string& audit) {
        d.allowed = false;
        d.action = FullDiveSafetyAction::EmergencyStop;
        d.severity = "critical";
        d.reason = reason;
        d.auditCode = audit;
        d.requiresEmergencyStop = true;
        d.requiresRealityAnchor = true;
    };

    // Mandatory order: locked-in prevention, no-pain guarantee, physiological/signal gates,
    // patient/adaptive envelopes, modality hard limiters, coherence/body schema, dose/budget,
    // device/chip, anchor and decompression controls.
    if (!frame.interoceptiveSafety.patientStopSignalAlive) {
        emergency("patient_stop_channel_lost", "FDSS_LOCKED_IN_PREVENTER_STOP_CHANNEL_LOST");
        return d;
    }
    if (!frame.safety.painSimulationBlocked) {
        emergency("pain_or_forbidden_perception_requested", "FDSS_NO_PAIN_VIOLATION");
        return d;
    }
    if (!frame.safety.clinicalModeLocked) {
        emergency("clinical_mode_not_locked", "FDSS_CLINICAL_MODE_UNLOCKED");
        return d;
    }
    if (!frame.safety.emergencyCutoffAvailable) {
        emergency("hardware_cutoff_unverified", "FDSS_CUTOFF_MISSING");
        return d;
    }
    if (frame.realityAnchor.anchorRequired && frame.realityAnchor.anchorIntensity <= 0.0f) {
        emergency("reality_anchor_hard_channel_missing", "FDSS_REALITY_ANCHOR_MISSING");
        return d;
    }
    if (frame.safety.patientCalibrationHash.empty()) {
        d.allowed = false;
        d.action = FullDiveSafetyAction::HoldLastSafeFrame;
        d.reason = "patient_specific_calibration_missing";
        d.severity = "high";
        d.auditCode = "FDSS_PATIENT_ENVELOPE_MISSING";
        d.requiresRealityAnchor = true;
        d.requiresRecalibration = true;
        d.maxAllowedIntensityScale = 0.0f;
        return d;
    }
    if (frame.vestibular.channels.size() > 0U && frame.visual.channels.empty()) {
        d.allowed = false;
        d.action = FullDiveSafetyAction::RealityAnchor;
        d.reason = "vestibular_without_visual_motion_anchor";
        d.severity = "high";
        d.auditCode = "FDSS_VESTIBULAR_HARD_LIMITER";
        d.modalitiesToSuppress.push_back(FullDiveModality::Vestibular);
        d.requiresRealityAnchor = true;
        d.maxAllowedIntensityScale = 0.25f;
        return d;
    }
    if (frame.safety.aggregateRisk >= 0.95f) {
        emergency("aggregate_risk_requires_emergency_stop", "FDSS_AGGREGATE_RISK_CRITICAL");
        d.requiresPhysicianCheck = true;
        return d;
    }
    if (frame.safety.aggregateRisk >= 0.80f) {
        d.allowed = false;
        d.action = FullDiveSafetyAction::Decompress;
        d.reason = "aggregate_risk_requires_decompression";
        d.severity = "high";
        d.maxAllowedIntensityScale = 0.25f;
        d.requiresRealityAnchor = true;
        d.requiresDecompression = true;
        d.auditCode = "FDSS_RISK_DECOMPRESSION";
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
