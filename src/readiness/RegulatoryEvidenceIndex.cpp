#include "RegulatoryEvidenceIndex.h"
#include <algorithm>

namespace dnxr {

void RegulatoryEvidenceIndex::addArtifact(const std::string& category, const std::string& artifact) {
    auto& items = artifacts_[category];
    if (std::find(items.begin(), items.end(), artifact) == items.end()) {
        items.push_back(artifact);
    }
}

bool RegulatoryEvidenceIndex::hasArtifact(const std::string& category, const std::string& artifact) const {
    const auto it = artifacts_.find(category);
    if (it == artifacts_.end()) return false;
    return std::find(it->second.begin(), it->second.end(), artifact) != it->second.end();
}

std::vector<std::string> RegulatoryEvidenceIndex::artifactsFor(const std::string& category) const {
    const auto it = artifacts_.find(category);
    return it == artifacts_.end() ? std::vector<std::string>{} : it->second;
}

std::size_t RegulatoryEvidenceIndex::artifactCount() const {
    std::size_t count = 0;
    for (const auto& entry : artifacts_) count += entry.second.size();
    return count;
}

} // namespace dnxr


namespace dnxr {

void RegulatoryEvidenceIndex::indexReviewedRepository(const EvidenceRepository& evidence) {
    for (const auto& artifact : evidence.artifacts()) {
        if (artifact.reviewState == EvidenceReviewState::Reviewed || artifact.reviewState == EvidenceReviewState::Signed) {
            addArtifact(artifact.type, artifact.id);
        }
        if (artifact.reviewState == EvidenceReviewState::Signed &&
            std::find(signedArtifactTypes_.begin(), signedArtifactTypes_.end(), artifact.type) == signedArtifactTypes_.end()) {
            signedArtifactTypes_.push_back(artifact.type);
        }
    }
}

bool RegulatoryEvidenceIndex::hasSignedArtifactType(const std::string& type) const {
    return std::find(signedArtifactTypes_.begin(), signedArtifactTypes_.end(), type) != signedArtifactTypes_.end();
}

} // namespace dnxr
