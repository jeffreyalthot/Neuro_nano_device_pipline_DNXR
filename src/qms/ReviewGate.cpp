#include "qms/ReviewGate.h"

namespace dnxr::qms {

ReviewGateDecision ReviewGate::evaluateHighRiskClosure(const ReviewGateInput& input) const {
    ReviewGateDecision decision;
    if (input.severity == "High" && !input.signoff.isValid()) {
        decision.blockers.push_back("high-severity hazard requires valid independent sign-off");
    }
    if (!input.artifact.isCompleteForClinicalReadiness()) {
        decision.blockers.push_back("clinical promotion requires a signed evidence artifact");
    }
    if (!input.review.isComplete()) {
        decision.blockers.push_back("review record must be complete and accepted");
    }
    if (!input.residualRiskAccepted) {
        decision.blockers.push_back("residual risk must be explicitly accepted");
    }
    if (!input.cybersecurityEvidenceIsExecuted) {
        decision.blockers.push_back("cybersecurity evidence must be executed evidence, not planning-only content");
    }
    decision.passed = decision.blockers.empty();
    return decision;
}

bool ReviewGate::blocksClinicalPromotion(const ReviewGateInput& input) const {
    return !evaluateHighRiskClosure(input).passed;
}

} // namespace dnxr::qms
