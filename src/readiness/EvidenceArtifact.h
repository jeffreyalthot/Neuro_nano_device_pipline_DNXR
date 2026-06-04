#pragma once

#include "readiness/EvidenceReviewState.h"

#include <string>

namespace dnxr {

struct EvidenceArtifact {
    std::string id;
    std::string path;
    std::string type;
    std::string linkedRequirement;
    std::string linkedHazard;
    EvidenceReviewState reviewState{EvidenceReviewState::Missing};
    std::string reviewer;
    std::string date;

    bool isCompleteForClinicalReadiness() const;
};

} // namespace dnxr
