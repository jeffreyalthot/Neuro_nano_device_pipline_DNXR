#include "ClinicalEvidenceVerifier.h"

#include <algorithm>

namespace dnxr {

std::vector<ClinicalEvidenceKind> ClinicalEvidenceVerifier::requiredHumanUseEvidence() {
    return {ClinicalEvidenceKind::PhysicalHILSession,
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
    result.complete = result.missing.empty() && result.invalid.empty();
    return result;
}

} // namespace dnxr
