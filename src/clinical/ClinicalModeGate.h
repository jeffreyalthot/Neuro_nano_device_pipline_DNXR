#pragma once

#include "ClinicalEvidenceVerifier.h"

#include <string>
#include <vector>

namespace dnxr {

struct ClinicalModeGateDecision {
    bool humanUseAllowed = false;
    bool stimulusDispatchAllowed = false;
    std::vector<std::string> blockers;
};

class ClinicalModeGate {
public:
    ClinicalModeGateDecision evaluateHumanUseRequest(const std::vector<ClinicalEvidenceRecord>& evidence,
                                                     std::int64_t nowUnixSeconds) const;
    bool canConnectClinicalDevice(const std::vector<ClinicalEvidenceRecord>& evidence,
                                  std::int64_t nowUnixSeconds) const;
};

} // namespace dnxr
