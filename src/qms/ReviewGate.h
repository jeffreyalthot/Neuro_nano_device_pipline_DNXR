#pragma once

#include "qms/ReviewRecord.h"
#include "qms/SignOffRecord.h"
#include "readiness/EvidenceArtifact.h"

#include <string>
#include <vector>

namespace dnxr::qms {

struct ReviewGateInput {
    std::string hazardId;
    std::string severity;
    dnxr::EvidenceArtifact artifact;
    ReviewRecord review;
    SignOffRecord signoff;
    bool residualRiskAccepted = false;
    bool cybersecurityEvidenceIsExecuted = false;
};

struct ReviewGateDecision {
    bool passed = false;
    std::vector<std::string> blockers;
};

class ReviewGate {
public:
    ReviewGateDecision evaluateHighRiskClosure(const ReviewGateInput& input) const;
    bool blocksClinicalPromotion(const ReviewGateInput& input) const;
};

} // namespace dnxr::qms
