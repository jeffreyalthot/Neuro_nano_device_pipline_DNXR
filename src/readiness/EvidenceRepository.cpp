#include "readiness/EvidenceRepository.h"

#include <utility>

namespace dnxr {

void EvidenceRepository::add(EvidenceArtifact artifact) { artifacts_.push_back(std::move(artifact)); }
std::vector<EvidenceArtifact> EvidenceRepository::artifacts() const { return artifacts_; }

std::vector<EvidenceArtifact> EvidenceRepository::forRequirement(const std::string& requirementId) const {
    std::vector<EvidenceArtifact> matches;
    for (const auto& artifact : artifacts_) {
        if (artifact.linkedRequirement == requirementId) {
            matches.push_back(artifact);
        }
    }
    return matches;
}

bool EvidenceRepository::hasSignedEvidenceForHazard(const std::string& hazardId) const {
    for (const auto& artifact : artifacts_) {
        if (artifact.linkedHazard == hazardId && artifact.isCompleteForClinicalReadiness()) {
            return true;
        }
    }
    return false;
}

} // namespace dnxr
