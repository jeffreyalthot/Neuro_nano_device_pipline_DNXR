#include "ClinicalUnlockManager.h"

namespace dnxr {

ClinicalUnlockCapabilities ClinicalUnlockManager::evaluate(const ClinicalUnlockContext& c) const {
    ClinicalUnlockCapabilities out;
    if (c.emergencyLatched) { out.status = "EMERGENCY_LOCKED"; out.blockers.push_back("emergency_stop_latched"); return out; }
    if (c.signatureRevoked) { out.status = "CLINICAL_TEST_REVOKED"; out.blockers.push_back("signature_revoked"); return out; }
    if (c.protocolExpired) { out.status = "CLINICAL_TEST_EXPIRED"; out.blockers.push_back("protocol_or_unlock_expired"); return out; }
    if (!c.qmsPackageValid || !c.clinicUnlockValid) { out.blockers.push_back("qms_or_clinic_unlock_invalid_or_missing"); return out; }

    out.clinicalReviewAllowed = true;
    out.status = "CLINICAL_REVIEW_UNLOCKED";
    out.reasons.push_back("QMS package valid");

    if (!(c.regulatoryAuthorizationValid && c.ethicsApprovalValid && c.physicianInvestigatorValid &&
          c.clinicalProtocolSigned && c.clinicalSiteAuthorized && c.operatorAuthorized)) {
        if (!c.regulatoryAuthorizationValid) out.blockers.push_back("regulatory_authorization_missing");
        if (!c.ethicsApprovalValid) out.blockers.push_back("ethics_approval_missing");
        if (!c.physicianInvestigatorValid) out.blockers.push_back("physician_investigator_missing");
        if (!c.clinicalProtocolSigned) out.blockers.push_back("clinical_protocol_not_signed");
        if (!c.clinicalSiteAuthorized) out.blockers.push_back("clinical_site_not_authorized");
        if (!c.operatorAuthorized) out.blockers.push_back("operator_not_authorized");
        return out;
    }

    out.status = "CLINICAL_TEST_UNLOCKED";
    out.humanConnectionAllowed = true;
    out.humanSessionAllowed = true;
    out.signalRecordingAllowed = true;
    out.calibrationAllowed = true;
    out.reasons.push_back("clinical site authorized");
    out.reasons.push_back("protocol signed");
    out.reasons.push_back("regulatory and ethics authorization present");
    out.reasons.push_back("physician investigator signed");

    if (!(c.sessionUnlockValid && c.patientConsentValid && c.patientCalibrationValid && c.sessionPrescriptionValid &&
          c.monitoringActive && c.softwareEmergencyStopReady && c.physicalEmergencyStopReady && c.sessionWithinProtocolLimits)) {
        if (!c.sessionUnlockValid) out.blockers.push_back("patient_session_not_loaded_or_invalid");
        if (!c.patientConsentValid) out.blockers.push_back("patient_consent_missing");
        if (!c.patientCalibrationValid) out.blockers.push_back("patient_calibration_missing");
        if (!c.sessionPrescriptionValid) out.blockers.push_back("session_prescription_missing");
        if (!c.monitoringActive) out.blockers.push_back("monitoring_not_active");
        if (!c.softwareEmergencyStopReady || !c.physicalEmergencyStopReady) out.blockers.push_back("emergency_stop_not_ready");
        if (!c.sessionWithinProtocolLimits) out.blockers.push_back("session_outside_protocol_limits");
        return out;
    }

    out.status = "CLINICAL_STIMULUS_DISPATCH_UNLOCKED";
    out.stimulusDispatchAllowed = true;
    out.reasons.push_back("patient session signed and within protocol limits");
    return out;
}

} // namespace dnxr
