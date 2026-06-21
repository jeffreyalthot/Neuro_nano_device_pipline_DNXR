#pragma once
#include <string>
#include <vector>

namespace dnxr {

struct RegulatoryAuthorizationEvidenceDecision {
    bool irbApprovalPresent = false;
    bool rebApprovalPresent = false;
    bool fdaIdeAuthorizationPresent = false;
    bool healthCanadaItaAuthorizationPresent = false;
    bool physicianInvestigatorSignoffPresent = false;
    bool hardwareBenchSignoffPresent = false;
    bool clinicalSiteAuthorizationPresent = false;
    bool qmsReleaseSignoffPresent = false;
    bool clinicalProtocolApprovalPresent = false;
    bool canadaPathValid = false;
    bool usaPathValid = false;
    bool regulatoryPathValid = false;
    bool completeExternalPackage = false;
    bool allowHumanUse = false;
    bool allowStimulusDispatch = false;
    std::vector<std::string> missingArtifacts;
    std::vector<std::string> invalidArtifacts;
};

class RegulatoryAuthorizationEvidenceGate {
public:
    explicit RegulatoryAuthorizationEvidenceGate(std::string root = ".");
    RegulatoryAuthorizationEvidenceDecision evaluate() const;
private:
    std::string root_;
};

} // namespace dnxr
