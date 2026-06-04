#pragma once

#include "ClinicalReadinessLevel.h"
#include "ReadinessScorecard.h"
#include "readiness/EvidenceRepository.h"

#include <string>
#include <vector>

namespace dnxr {

struct ClinicalReadinessAssessment {
    ClinicalReadinessLevel level = ClinicalReadinessLevel::SimulationOnly;
    double score = 0.0;
    std::vector<std::string> gaps;
    bool clinicalTestReady = false;
};

class ClinicalTestReadinessAssessor {
public:
    ClinicalReadinessAssessment assess(const ReadinessScorecard& scorecard) const;
    ClinicalReadinessAssessment assess(const ReadinessScorecard& scorecard, const EvidenceRepository& evidence) const;
    bool canReturnClinicalTestReady(const ReadinessScorecard& scorecard) const;
    bool canReturnClinicalTestReady(const ReadinessScorecard& scorecard, const EvidenceRepository& evidence) const;
};

} // namespace dnxr
