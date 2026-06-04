#include "ReadinessScorecard.h"

namespace dnxr {

int ReadinessScorecard::completedItems() const {
    int count = 0;
    const bool items[] = {fullDiveCapabilityEvidence, integratedSimulationEvidence, hilEvidenceComplete,
                          riskManagementComplete, traceabilityComplete, cybersecurityDocsComplete,
                          verificationValidationComplete, qmsDocsComplete, physicianReviewPackageComplete,
                          canadaUsReadinessDocsComplete, protocolReadinessPackageComplete};
    for (bool item : items) {
        count += item ? 1 : 0;
    }
    return count;
}

int ReadinessScorecard::totalItems() const { return 11; }

double ReadinessScorecard::completionRatio() const {
    return static_cast<double>(completedItems()) / static_cast<double>(totalItems());
}

std::vector<std::string> ReadinessScorecard::missingClinicalTestReadyItems() const {
    std::vector<std::string> missing;
    if (!fullDiveCapabilityEvidence) missing.push_back("full-dive capability evidence");
    if (!integratedSimulationEvidence) missing.push_back("integrated simulation evidence");
    if (!hilEvidenceComplete) missing.push_back("HIL evidence complete");
    if (!riskManagementComplete) missing.push_back("risk management complete");
    if (!traceabilityComplete) missing.push_back("traceability complete");
    if (!cybersecurityDocsComplete) missing.push_back("cybersecurity docs complete");
    if (!verificationValidationComplete) missing.push_back("V&V evidence complete");
    if (!qmsDocsComplete) missing.push_back("QMS docs complete");
    if (!physicianReviewPackageComplete) missing.push_back("physician review package complete");
    if (!canadaUsReadinessDocsComplete) missing.push_back("Canada/US readiness docs complete");
    if (!protocolReadinessPackageComplete) missing.push_back("protocol readiness package complete");
    return missing;
}

} // namespace dnxr
