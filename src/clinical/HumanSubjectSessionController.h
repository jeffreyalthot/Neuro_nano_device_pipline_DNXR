#pragma once

#include "ClinicalModeGate.h"

#include <string>
#include <vector>

namespace dnxr {

struct HumanSubjectSessionPrerequisites {
    bool informedConsentSigned = false;
    bool independentPhysicianPresent = false;
    bool rebIrbApproved = false;
    bool regulatoryAuthorizationPresent = false;
    bool emergencyStopVerified = false;
    bool externalPhysicalStopVerified = false;
    bool continuousMonitoringAvailable = false;
    bool communicationChannelAvailable = false;
    bool signedCalibrationAvailable = false;
    bool postSessionFollowupScheduled = false;
};

struct HumanSubjectSessionDecision {
    bool mayStart = false;
    std::vector<std::string> blockers;
};

class HumanSubjectSessionController {
public:
    HumanSubjectSessionDecision validateStart(const HumanSubjectSessionPrerequisites& prerequisites,
                                              const std::vector<ClinicalEvidenceRecord>& evidence,
                                              std::int64_t nowUnixSeconds) const;
};

} // namespace dnxr
