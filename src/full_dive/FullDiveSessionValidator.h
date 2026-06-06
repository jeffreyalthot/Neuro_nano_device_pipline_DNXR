#pragma once
#include <string>
#include <vector>
namespace dnxr {

enum class FullDiveSessionType { SimulationBench, HumanSubject };

struct FullDiveSessionEvidence {
    bool emergencyExitAvailable=false;
    bool realityAnchorAvailable=false;
    bool decompressionAvailable=false;
    bool calibrationStable=false;
    bool temporalCoherenceMaintained=false;
    double sensoryExposureRatio=1.0;
    FullDiveSessionType sessionType=FullDiveSessionType::SimulationBench;
    bool informedConsentSigned=false;
    bool physicianPresent=false;
    bool protocolApproved=false;
    bool continuousMonitoring=false;
    bool independentEmergencyPath=false;
    bool doseWithinLimit=false;
    bool postSessionFollowupScheduled=false;
    bool tamperEvidentAuditLog=false;
    bool signedPatientCalibration=false;
};
struct FullDiveSessionValidationResult { bool valid=false; std::vector<std::string> failures; };
class FullDiveSessionValidator { public: FullDiveSessionValidationResult validate(const FullDiveSessionEvidence& evidence) const; };
}
