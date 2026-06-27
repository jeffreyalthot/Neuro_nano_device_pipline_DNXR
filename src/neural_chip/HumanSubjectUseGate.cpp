#include "neural_chip/HumanSubjectUseGate.h"

namespace dnxr {

HumanSubjectUseDecision HumanSubjectUseGate::evaluate(const HumanSubjectUseEvidence& e) const {
    HumanSubjectUseDecision d;
    d.allowSimulation = true;
    d.allowHil = e.hilEvidencePresent;
    d.allowHardwareBench = e.benchProfilePresent;

    if (e.emergencyLatched) {
        d.status = HumanSubjectUseStatus::EmergencyLocked;
        d.blockers.push_back("emergency_stop_latched");
        d.reason = "Emergency stop is latched; clinical use and stimulus dispatch are locked.";
        return d;
    }

    if (!e.qmsReleaseSignatureValid) {
        if (e.benchProfilePresent && e.hilEvidencePresent) {
            d.status = HumanSubjectUseStatus::PreclinicalReviewCandidate;
            d.allowClinicalReview = true;
            d.blockers.push_back("qms_release_signature_invalid_or_missing");
            d.reason =
                "Preclinical review candidate: bench and HIL evidence are present, "
                "but human use remains locked until signed QMS release evidence is valid.";
            return d;
        }
        d.status = HumanSubjectUseStatus::BlockedSimulationOnly;
        d.blockers.push_back("qms_release_signature_invalid_or_missing");
        d.reason = "Simulation-only lock: QMS release signature is missing or invalid.";
        return d;
    }
    if (!e.qmsDocumentsComplete) {
        d.status = HumanSubjectUseStatus::BlockedSimulationOnly;
        d.blockers.push_back("qms_documents_incomplete");
        d.reason = "Simulation-only lock: QMS document package is incomplete.";
        return d;
    }
    if (!e.qmsBuildHashMatches) {
        d.status = HumanSubjectUseStatus::BlockedSimulationOnly;
        d.blockers.push_back("qms_build_hash_mismatch");
        d.reason = "Simulation-only lock: QMS build hash does not match this release.";
        return d;
    }
    if (!e.qmsNotExpired || e.signatureExpired) {
        d.status = HumanSubjectUseStatus::ClinicalTestExpired;
        d.blockers.push_back("qms_or_clinical_unlock_expired");
        d.reason = "Clinical unlock expired; obtain a fresh signed release/session package.";
        return d;
    }
    if (!e.qmsNotRevoked || e.signatureRevoked) {
        d.status = HumanSubjectUseStatus::ClinicalTestRevoked;
        d.blockers.push_back("qms_or_clinical_unlock_revoked");
        d.reason = "Clinical unlock revoked; runtime remains locked.";
        return d;
    }

    d.status = HumanSubjectUseStatus::ClinicalTestReady;
    d.allowClinicalReview = true;
    d.reason = "Clinical review mode unlocked; human-subject activity requires regulatory, ethics, protocol, site, and role evidence.";

    if (!(e.regulatoryAuthorizationPresent && e.ethicsApprovalPresent)) {
        d.status = HumanSubjectUseStatus::RegulatoryAuthorizationRequired;
        d.blockers.push_back("clinical_regulatory_or_ethics_authorization_missing");
        return d;
    }
    if (!e.physicianSignoffPresent) { d.blockers.push_back("physician_investigator_signoff_missing"); return d; }
    if (!e.hardwareBenchSignoffPresent) { d.blockers.push_back("hardware_bench_signoff_missing"); return d; }
    if (!e.clinicalProtocolSigned) { d.blockers.push_back("clinical_protocol_not_signed"); return d; }
    if (!e.clinicalSiteAuthorized || !e.operatorAuthorized) { d.blockers.push_back("clinical_site_or_operator_not_authorized"); return d; }
    if (e.openCriticalRisk || e.openBlockingCapa) { d.blockers.push_back("open_critical_risk_or_blocking_capa"); return d; }

    d.status = HumanSubjectUseStatus::ClinicalTestUnlocked;
    d.allowHumanUse = true;
    d.allowSignalRecording = true;
    d.allowCalibration = true;
    d.allowStimulusDispatch = false;
    d.reason = "Clinical human-use test mode unlocked. Stimulus dispatch still requires patient/session-specific unlock.";

    if (!e.patientConsentSigned) { d.blockers.push_back("patient_consent_missing"); return d; }
    if (!e.patientCalibrationSigned) { d.blockers.push_back("patient_calibration_missing"); return d; }
    if (!e.sessionPrescriptionSigned || !e.sessionWithinProtocolLimits) { d.blockers.push_back("session_unlock_missing_or_outside_protocol"); return d; }
    if (!e.continuousMonitoringActive) { d.blockers.push_back("continuous_monitoring_not_active"); return d; }
    if (!e.softwareEmergencyStopReady || !e.physicalEmergencyStopReady) { d.blockers.push_back("emergency_stop_not_ready"); return d; }
    if (!e.clinicianPresent) { d.blockers.push_back("clinician_not_present"); return d; }

    d.status = HumanSubjectUseStatus::ClinicalStimulusDispatchUnlocked;
    d.allowStimulusDispatch = true;
    d.reason = "Clinical human test mode fully unlocked for this signed session.";
    return d;
}

const char* toString(HumanSubjectUseStatus status) {
    switch (status) {
    case HumanSubjectUseStatus::BlockedSimulationOnly: return "BlockedSimulationOnly";
    case HumanSubjectUseStatus::PreclinicalReviewCandidate: return "PreclinicalReviewCandidate";
    case HumanSubjectUseStatus::BenchHilOnly: return "BenchHilOnly";
    case HumanSubjectUseStatus::RegulatoryAuthorizationRequired: return "RegulatoryAuthorizationRequired";
    case HumanSubjectUseStatus::ExternalAuthorizationPresent: return "ExternalAuthorizationPresent";
    case HumanSubjectUseStatus::ClinicalTestReady: return "ClinicalTestReady";
    case HumanSubjectUseStatus::ClinicalTestUnlocked: return "ClinicalTestUnlocked";
    case HumanSubjectUseStatus::ClinicalStimulusDispatchUnlocked: return "ClinicalStimulusDispatchUnlocked";
    case HumanSubjectUseStatus::ClinicalTestExpired: return "ClinicalTestExpired";
    case HumanSubjectUseStatus::ClinicalTestRevoked: return "ClinicalTestRevoked";
    case HumanSubjectUseStatus::EmergencyLocked: return "EmergencyLocked";
    }
    return "Unknown";
}

} // namespace dnxr
