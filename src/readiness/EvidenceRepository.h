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
    std::vector<EvidenceArtifact> forType(const std::string& type) const;
    bool hasSignedEvidenceForHazard(const std::string& hazardId) const;
    bool hasSignedEvidenceForRequirement(const std::string& requirementId) const;
    bool hasSignedEvidenceOfType(const std::string& type) const;
    bool hasReviewedOrSignedEvidenceOfType(const std::string& type) const;
    bool hasCompleteEvidenceForType(const std::string& type) const;
    bool hasUnsignedOrGeneratedOnlyEvidenceOfType(const std::string& type) const;

private:
    std::vector<EvidenceArtifact> artifacts_;
};

} // namespace dnxr
