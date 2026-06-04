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

std::vector<EvidenceArtifact> EvidenceRepository::forType(const std::string& type) const {
    std::vector<EvidenceArtifact> matches;
    for (const auto& artifact : artifacts_) {
        if (artifact.type == type) {
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

bool EvidenceRepository::hasSignedEvidenceForRequirement(const std::string& requirementId) const {
    for (const auto& artifact : artifacts_) {
        if (artifact.linkedRequirement == requirementId && artifact.isCompleteForClinicalReadiness()) {
            return true;
        }
    }
    return false;
}

bool EvidenceRepository::hasSignedEvidenceOfType(const std::string& type) const {
    for (const auto& artifact : artifacts_) {
        if (artifact.type == type && artifact.isCompleteForClinicalReadiness()) {
            return true;
        }
    }
    return false;
}

bool EvidenceRepository::hasReviewedOrSignedEvidenceOfType(const std::string& type) const {
    for (const auto& artifact : artifacts_) {
        if (artifact.type != type) continue;
        const bool reviewedOrSigned = artifact.reviewState == EvidenceReviewState::Reviewed || artifact.reviewState == EvidenceReviewState::Signed;
        if (reviewedOrSigned && !artifact.id.empty() && !artifact.path.empty() && !artifact.linkedRequirement.empty() &&
            !artifact.linkedHazard.empty() && !artifact.reviewer.empty() && !artifact.date.empty()) {
            return true;
        }
    }
    return false;
}

bool EvidenceRepository::hasCompleteEvidenceForType(const std::string& type) const {
    return hasSignedEvidenceOfType(type);
}

bool EvidenceRepository::hasUnsignedOrGeneratedOnlyEvidenceOfType(const std::string& type) const {
    for (const auto& artifact : artifacts_) {
        if (artifact.type == type && artifact.reviewState != EvidenceReviewState::Signed) {
            return true;
        }
    }
    return false;
}

} // namespace dnxr
