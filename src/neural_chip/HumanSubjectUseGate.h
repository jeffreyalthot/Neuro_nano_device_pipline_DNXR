#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class HumanSubjectUseStatus {
    BlockedSimulationOnly,
    PreclinicalReviewCandidate,
    BenchHilOnly,
    RegulatoryAuthorizationRequired,
    ExternalAuthorizationPresent,
    ClinicalTestReady,
    ClinicalTestUnlocked,
    ClinicalStimulusDispatchUnlocked,
    ClinicalTestExpired,
    ClinicalTestRevoked,
    EmergencyLocked
};

struct HumanSubjectUseDecision {
    HumanSubjectUseStatus status = HumanSubjectUseStatus::BlockedSimulationOnly;
    bool allowSimulation = true;
    bool allowHil = false;
    bool allowHardwareBench = false;
    bool allowClinicalReview = false;
    bool allowHumanUse = false;
    bool allowSignalRecording = false;
    bool allowCalibration = false;
    bool allowStimulusDispatch = false;
    std::string reason;
    std::vector<std::string> blockers;
};

struct HumanSubjectUseEvidence {
    bool simulationOnly = true;
    bool benchProfilePresent = false;
    bool hilEvidencePresent = false;

    bool qmsReleaseSignatureValid = false;
    bool qmsDocumentsComplete = false;
    bool qmsBuildHashMatches = false;
    bool qmsNotExpired = false;
    bool qmsNotRevoked = false;

    bool regulatoryAuthorizationPresent = false;
    bool ethicsApprovalPresent = false;
    bool physicianSignoffPresent = false;
    bool hardwareBenchSignoffPresent = false;

    bool clinicalProtocolSigned = false;
    bool clinicalSiteAuthorized = false;
    bool operatorAuthorized = false;

    bool patientConsentSigned = false;
    bool patientCalibrationSigned = false;
    bool sessionPrescriptionSigned = false;
    bool sessionWithinProtocolLimits = false;

    bool continuousMonitoringActive = false;
    bool softwareEmergencyStopReady = false;
    bool physicalEmergencyStopReady = false;
    bool clinicianPresent = false;

    bool openCriticalRisk = false;
    bool openBlockingCapa = false;
    bool signatureExpired = false;
    bool signatureRevoked = false;
    bool emergencyLatched = false;
};

class HumanSubjectUseGate {
public:
    HumanSubjectUseDecision evaluate(const HumanSubjectUseEvidence& evidence = {}) const;
};

const char* toString(HumanSubjectUseStatus status);

} // namespace dnxr
