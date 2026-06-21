#include "ClinicalModeGate.h"

namespace dnxr {

ClinicalModeGateDecision ClinicalModeGate::evaluateHumanUseRequest(
    const std::vector<ClinicalEvidenceRecord>& evidence,
    std::int64_t nowUnixSeconds) const {
    ClinicalEvidenceVerifier verifier;
    const auto verification = verifier.verify(evidence, nowUnixSeconds);
    ClinicalModeGateDecision decision;
    for (const auto& missing : verification.missing) decision.blockers.push_back("missing_signed_evidence:" + missing);
    for (const auto& invalid : verification.invalid) decision.blockers.push_back("invalid_or_expired_signed_evidence:" + invalid);

    decision.clinicalReviewAllowed = verification.qmsPackageValid;
    decision.humanConnectionAllowed = verification.regulatoryPackageValid && verification.ethicsPackageValid &&
                                      verification.physicianPackageValid && verification.clinicalProtocolValid &&
                                      verification.clinicalSiteAuthorized;
    decision.humanSessionAllowed = decision.humanConnectionAllowed;
    decision.humanUseAllowed = decision.humanSessionAllowed;
    decision.stimulusDispatchAllowed = decision.humanSessionAllowed && verification.sessionUnlockValid &&
                                       verification.patientConsentValid && verification.patientCalibrationValid &&
                                       verification.monitoringActive && verification.emergencyStopReady;
    if (decision.humanSessionAllowed && !decision.stimulusDispatchAllowed) {
        decision.blockers.push_back("stimulus_dispatch_requires_signed_session_consent_calibration_monitoring_and_emergency_stops");
    }
    return decision;
}

bool ClinicalModeGate::canConnectClinicalDevice(const std::vector<ClinicalEvidenceRecord>& evidence,
                                                std::int64_t nowUnixSeconds) const {
    return evaluateHumanUseRequest(evidence, nowUnixSeconds).humanConnectionAllowed;
}

} // namespace dnxr
