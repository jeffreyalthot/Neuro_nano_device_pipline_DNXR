#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct ClinicalUnlockCapabilities {
    bool clinicalReviewAllowed = false;
    bool humanConnectionAllowed = false;
    bool humanSessionAllowed = false;
    bool signalRecordingAllowed = false;
    bool calibrationAllowed = false;
    bool stimulusDispatchAllowed = false;
    std::string status = "BLOCKED";
    std::vector<std::string> blockers;
    std::vector<std::string> reasons;
};

struct ClinicalUnlockContext {
    bool qmsPackageValid = false;
    bool clinicUnlockValid = false;
    bool sessionUnlockValid = false;
    bool regulatoryAuthorizationValid = false;
    bool ethicsApprovalValid = false;
    bool physicianInvestigatorValid = false;
    bool clinicalProtocolSigned = false;
    bool clinicalSiteAuthorized = false;
    bool operatorAuthorized = false;
    bool patientConsentValid = false;
    bool patientCalibrationValid = false;
    bool sessionPrescriptionValid = false;
    bool monitoringActive = false;
    bool softwareEmergencyStopReady = false;
    bool physicalEmergencyStopReady = false;
    bool sessionWithinProtocolLimits = false;
    bool protocolExpired = false;
    bool signatureRevoked = false;
    bool emergencyLatched = false;
};

} // namespace dnxr
