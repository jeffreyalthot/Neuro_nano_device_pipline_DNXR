#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct ReadinessScorecard {
    bool fullDiveCapabilityEvidence = false;
    bool integratedSimulationEvidence = false;
    bool hilEvidenceComplete = false;
    bool riskManagementComplete = false;
    bool traceabilityComplete = false;
    bool cybersecurityDocsComplete = false;
    bool verificationValidationComplete = false;
    bool qmsDocsComplete = false;
    bool physicianReviewPackageComplete = false;
    bool canadaUsReadinessDocsComplete = false;
    bool protocolReadinessPackageComplete = false;

    int completedItems() const;
    int totalItems() const;
    double completionRatio() const;
    std::vector<std::string> missingClinicalTestReadyItems() const;
};

} // namespace dnxr
