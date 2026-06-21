#include "ClinicalEvidenceVerifier.h"

#include <algorithm>

namespace dnxr {

std::vector<ClinicalEvidenceKind> ClinicalEvidenceVerifier::requiredHumanUseEvidence() {
    return {ClinicalEvidenceKind::QMSReleaseSignoff,
            ClinicalEvidenceKind::PhysicalHILSession,
            ClinicalEvidenceKind::PreclinicalSafetyReport,
            ClinicalEvidenceKind::RiskControlVerification,
            ClinicalEvidenceKind::ClinicalProtocolPackage,
            ClinicalEvidenceKind::REBIRBApproval,
            ClinicalEvidenceKind::HealthCanadaITAOrIDEAuthorization,
            ClinicalEvidenceKind::IndependentMedicalSignoff};
}

const char* ClinicalEvidenceVerifier::toString(ClinicalEvidenceKind kind) {
    switch (kind) {
    case ClinicalEvidenceKind::PhysicalHILSession: return "physical_hil_session";
    case ClinicalEvidenceKind::PreclinicalSafetyReport: return "preclinical_safety_report";
    case ClinicalEvidenceKind::RiskControlVerification: return "risk_control_verification";
    case ClinicalEvidenceKind::ClinicalProtocolPackage: return "clinical_protocol_package";
    case ClinicalEvidenceKind::REBIRBApproval: return "REB_IRB_approval";
    case ClinicalEvidenceKind::HealthCanadaITAOrIDEAuthorization: return "Health_Canada_ITA_or_IDE_authorization";
    case ClinicalEvidenceKind::IndependentMedicalSignoff: return "independent_medical_signoff";
    case ClinicalEvidenceKind::QMSReleaseSignoff: return "qms_release_signoff";
    case ClinicalEvidenceKind::ClinicalSiteAuthorization: return "clinical_site_authorization";
    case ClinicalEvidenceKind::OperatorAuthorization: return "operator_authorization";
    case ClinicalEvidenceKind::PatientConsent: return "patient_consent";
    case ClinicalEvidenceKind::PatientCalibration: return "patient_calibration";
    case ClinicalEvidenceKind::SessionPrescription: return "session_prescription";
    case ClinicalEvidenceKind::MonitoringActivation: return "monitoring_activation";
    case ClinicalEvidenceKind::EmergencyStopVerification: return "emergency_stop_verification";
    }
    return "unknown";
}

ClinicalEvidenceVerificationResult ClinicalEvidenceVerifier::verify(
    const std::vector<ClinicalEvidenceRecord>& records,
    std::int64_t nowUnixSeconds) const {
    ClinicalEvidenceVerificationResult result;
    for (const auto required : requiredHumanUseEvidence()) {
        auto it = std::find_if(records.begin(), records.end(), [required](const ClinicalEvidenceRecord& record) {
            return record.kind == required;
        });
        if (it == records.end()) {
            result.missing.push_back(toString(required));
            continue;
        }
        if (it->artifactId.empty() || it->hash.size() < 16U || it->signature.size() < 16U ||
            it->author.empty() || it->role.empty() || !it->approved ||
            it->expiresAtUnixSeconds <= nowUnixSeconds || it->signedAtUnixSeconds <= 0) {
            result.invalid.push_back(toString(required));
        }
    }
    auto valid = [&](ClinicalEvidenceKind kind) {
        return std::find_if(records.begin(), records.end(), [kind, nowUnixSeconds](const ClinicalEvidenceRecord& record) {
            return record.kind == kind && !record.artifactId.empty() && record.hash.size() >= 16U &&
                   record.signature.size() >= 16U && !record.author.empty() && !record.role.empty() &&
                   record.approved && record.expiresAtUnixSeconds > nowUnixSeconds && record.signedAtUnixSeconds > 0;
        }) != records.end();
    };
    result.qmsPackageValid = valid(ClinicalEvidenceKind::QMSReleaseSignoff) &&
                             valid(ClinicalEvidenceKind::RiskControlVerification) &&
                             valid(ClinicalEvidenceKind::PreclinicalSafetyReport);
    result.regulatoryPackageValid = valid(ClinicalEvidenceKind::HealthCanadaITAOrIDEAuthorization);
    result.ethicsPackageValid = valid(ClinicalEvidenceKind::REBIRBApproval);
    result.physicianPackageValid = valid(ClinicalEvidenceKind::IndependentMedicalSignoff);
    result.clinicalProtocolValid = valid(ClinicalEvidenceKind::ClinicalProtocolPackage);
    result.clinicalSiteAuthorized = valid(ClinicalEvidenceKind::ClinicalSiteAuthorization);
    result.sessionUnlockValid = valid(ClinicalEvidenceKind::SessionPrescription);
    result.patientConsentValid = valid(ClinicalEvidenceKind::PatientConsent);
    result.patientCalibrationValid = valid(ClinicalEvidenceKind::PatientCalibration);
    result.monitoringActive = valid(ClinicalEvidenceKind::MonitoringActivation);
    result.emergencyStopReady = valid(ClinicalEvidenceKind::EmergencyStopVerification);
    result.complete = result.missing.empty() && result.invalid.empty();
    return result;
}

} // namespace dnxr
