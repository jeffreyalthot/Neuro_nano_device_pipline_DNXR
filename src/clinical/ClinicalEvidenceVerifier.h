#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class ClinicalEvidenceKind {
    PhysicalHILSession,
    PreclinicalSafetyReport,
    RiskControlVerification,
    ClinicalProtocolPackage,
    REBIRBApproval,
    HealthCanadaITAOrIDEAuthorization,
    IndependentMedicalSignoff,
    QMSReleaseSignoff,
    ClinicalSiteAuthorization,
    OperatorAuthorization,
    PatientConsent,
    PatientCalibration,
    SessionPrescription,
    MonitoringActivation,
    EmergencyStopVerification
};

struct ClinicalEvidenceRecord {
    ClinicalEvidenceKind kind = ClinicalEvidenceKind::PhysicalHILSession;
    std::string artifactId;
    std::string hash;
    std::string signature;
    std::string author;
    std::string role;
    bool approved = false;
    std::int64_t signedAtUnixSeconds = 0;
    std::int64_t expiresAtUnixSeconds = 0;
};

struct ClinicalEvidenceVerificationResult {
    bool complete = false;
    bool qmsPackageValid = false;
    bool regulatoryPackageValid = false;
    bool ethicsPackageValid = false;
    bool physicianPackageValid = false;
    bool clinicalProtocolValid = false;
    bool clinicalSiteAuthorized = false;
    bool sessionUnlockValid = false;
    bool patientConsentValid = false;
    bool patientCalibrationValid = false;
    bool monitoringActive = false;
    bool emergencyStopReady = false;
    std::vector<std::string> missing;
    std::vector<std::string> invalid;
};

class ClinicalEvidenceVerifier {
public:
    ClinicalEvidenceVerificationResult verify(const std::vector<ClinicalEvidenceRecord>& records,
                                              std::int64_t nowUnixSeconds) const;
    static std::vector<ClinicalEvidenceKind> requiredHumanUseEvidence();
    static const char* toString(ClinicalEvidenceKind kind);
};

} // namespace dnxr
