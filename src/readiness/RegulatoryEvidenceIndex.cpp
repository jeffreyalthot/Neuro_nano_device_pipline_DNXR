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
