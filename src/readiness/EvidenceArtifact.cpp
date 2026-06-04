#include "readiness/EvidenceArtifact.h"

namespace dnxr {

bool EvidenceArtifact::isCompleteForClinicalReadiness() const {
    return !id.empty() && !path.empty() && !type.empty() && !linkedRequirement.empty() && !linkedHazard.empty() &&
           reviewState == EvidenceReviewState::Signed && !reviewer.empty() && !date.empty();
}

} // namespace dnxr
