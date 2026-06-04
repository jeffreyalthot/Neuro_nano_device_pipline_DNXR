#pragma once

#include "readiness/EvidenceRepository.h"

#include <string>
#include <vector>

namespace dnxr {

struct PreclinicalEvidenceChecklist {
    bool fullDiveSessionValidated = false;
    bool hilFaultInjectionPassed = false;
    bool latencyBudgetPassed = false;
    bool emergencyStopPassed = false;
    bool recoverySequencePassed = false;
    bool riskControlsVerified = false;
};

struct PreclinicalReadinessReport {
    bool ready = false;
    std::vector<std::string> gaps;
};

class PreclinicalReadinessAssessor {
public:
    PreclinicalReadinessReport assess(const PreclinicalEvidenceChecklist& checklist) const;
    PreclinicalReadinessReport assess(const PreclinicalEvidenceChecklist& checklist, const EvidenceRepository& evidence) const;
};

} // namespace dnxr
