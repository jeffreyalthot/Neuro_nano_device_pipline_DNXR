#include "FullDiveSessionValidator.h"
namespace dnxr {
FullDiveSessionValidationResult FullDiveSessionValidator::validate(const FullDiveSessionEvidence& e) const {
    FullDiveSessionValidationResult r;
    if(!e.emergencyExitAvailable) r.failures.push_back("emergency exit missing");
    if(!e.realityAnchorAvailable) r.failures.push_back("reality anchor missing");
    if(!e.decompressionAvailable) r.failures.push_back("decompression missing");
    if(!e.calibrationStable) r.failures.push_back("calibration unstable");
    if(!e.temporalCoherenceMaintained) r.failures.push_back("temporal coherence lost");
    if(e.sensoryExposureRatio>1.0) r.failures.push_back("sensory exposure exceeded");
    if(e.sessionType == FullDiveSessionType::HumanSubject) {
        if(!e.informedConsentSigned) r.failures.push_back("consent missing");
        if(!e.physicianPresent) r.failures.push_back("physician missing");
        if(!e.protocolApproved) r.failures.push_back("protocol approval missing");
        if(!e.continuousMonitoring) r.failures.push_back("continuous monitoring missing");
        if(!e.independentEmergencyPath) r.failures.push_back("independent emergency path missing");
        if(!e.doseWithinLimit) r.failures.push_back("dose limit evidence missing");
        if(!e.postSessionFollowupScheduled) r.failures.push_back("post-session followup missing");
        if(!e.tamperEvidentAuditLog) r.failures.push_back("audit log missing");
        if(!e.signedPatientCalibration) r.failures.push_back("signed calibration missing");
    }
    r.valid=r.failures.empty();
    return r;
}
}
