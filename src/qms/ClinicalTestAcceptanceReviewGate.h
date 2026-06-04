#pragma once

#include "readiness/EvidenceRepository.h"
#include "readiness/ReadinessScorecard.h"

#include <string>
#include <vector>

namespace dnxr::qms {

struct ClinicalTestAcceptanceReviewDecision {
    bool readyForExpertAcceptanceReview = false;
    bool readyToBeginClinicalTesting = false;
    std::vector<std::string> acceptanceReviewBlockers;
    std::vector<std::string> clinicalTestingBlockers;
};

class ClinicalTestAcceptanceReviewGate {
public:
    ClinicalTestAcceptanceReviewDecision evaluate(const ReadinessScorecard& scorecard,
                                                  const EvidenceRepository& evidence) const;
};

} // namespace dnxr::qms
