#pragma once

#include "readiness/EvidenceArtifact.h"

#include <string>
#include <vector>

namespace dnxr {

class EvidenceRepository {
public:
    void add(EvidenceArtifact artifact);
    std::vector<EvidenceArtifact> artifacts() const;
    std::vector<EvidenceArtifact> forRequirement(const std::string& requirementId) const;
    bool hasSignedEvidenceForHazard(const std::string& hazardId) const;

private:
    std::vector<EvidenceArtifact> artifacts_;
};

} // namespace dnxr
