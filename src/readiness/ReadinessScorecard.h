#pragma once

#include "readiness/EvidenceRepository.h"

#include <string>
#include <vector>

namespace dnxr {

enum class ReadinessDomainStatus {
    Blocked,
    EvidenceOpen,
    PreclinicalReady,
    LockedComplete,
    ApprovalRequestReadyHumanUseLocked,
    HumanUseReady
};

struct SeniorMedicalReadinessDomain {
    std::string domain;
    double score = 0.0;
    ReadinessDomainStatus status = ReadinessDomainStatus::EvidenceOpen;
    std::string evidenceGate;
    bool blocksHumanUse = false;
};

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
    std::vector<SeniorMedicalReadinessDomain> seniorMedicalDomainRatings() const;
    std::vector<SeniorMedicalReadinessDomain> seniorMedicalDomainRatings(const EvidenceRepository& evidence) const;
    std::string seniorMedicalMarkdownTable() const;
    std::string seniorMedicalMarkdownTable(const EvidenceRepository& evidence) const;
};

const char* toString(ReadinessDomainStatus status);

} // namespace dnxr
