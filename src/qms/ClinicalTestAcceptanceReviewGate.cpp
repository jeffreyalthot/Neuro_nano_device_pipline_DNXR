#include "qms/ClinicalTestAcceptanceReviewGate.h"

#include <array>

namespace dnxr::qms {
namespace {

bool scorecardSupportsAcceptanceReview(const ReadinessScorecard& scorecard) {
    return scorecard.fullDiveCapabilityEvidence && scorecard.integratedSimulationEvidence && scorecard.hilEvidenceComplete &&
           scorecard.riskManagementComplete && scorecard.traceabilityComplete && scorecard.cybersecurityDocsComplete &&
           scorecard.verificationValidationComplete && scorecard.qmsDocsComplete && scorecard.physicianReviewPackageComplete &&
           scorecard.canadaUsReadinessDocsComplete && scorecard.protocolReadinessPackageComplete;
}

void addMissingReviewedArtifacts(std::vector<std::string>& blockers,
                                 const EvidenceRepository& evidence,
                                 const std::array<const char*, 5>& requiredTypes) {
    for (const char* type : requiredTypes) {
        if (!evidence.hasReviewedOrSignedEvidenceOfType(type)) {
            blockers.push_back(std::string("reviewed or signed acceptance-review evidence required: ") + type);
        }
    }
}

void addMissingSignedArtifacts(std::vector<std::string>& blockers,
                               const EvidenceRepository& evidence,
                               const std::array<const char*, 18>& requiredTypes) {
    for (const char* type : requiredTypes) {
        if (!evidence.hasSignedEvidenceOfType(type)) {
            blockers.push_back(std::string("signed clinical-test evidence required: ") + type);
        }
    }
}

} // namespace

ClinicalTestAcceptanceReviewDecision ClinicalTestAcceptanceReviewGate::evaluate(const ReadinessScorecard& scorecard,
                                                                                 const EvidenceRepository& evidence) const {
    ClinicalTestAcceptanceReviewDecision decision;

    if (!scorecardSupportsAcceptanceReview(scorecard)) {
        for (const auto& gap : scorecard.missingClinicalTestReadyItems()) {
            decision.acceptanceReviewBlockers.push_back("scorecard item incomplete: " + gap);
        }
    }

    constexpr std::array<const char*, 5> acceptanceReviewEvidence = {
        "software_hil_session",
        "runtime_10k_full",
        "cybersecurity_package",
        "qms_signoff_packets",
        "clinical_external_blockers",
    };
    addMissingReviewedArtifacts(decision.acceptanceReviewBlockers, evidence, acceptanceReviewEvidence);
    decision.readyForExpertAcceptanceReview = decision.acceptanceReviewBlockers.empty();

    decision.clinicalTestingBlockers = decision.acceptanceReviewBlockers;
    constexpr std::array<const char*, 18> clinicalTestEvidence = {
        "physical_hil_session",
        "physical_hil_raw_logs",
        "phantom_qualification_report",
        "target_hardware_latency_report",
        "risk_control_verification",
        "runtime_10k_full",
        "cybersecurity_executed",
        "clinical_protocol_package",
        "reb_irb_approval",
        "physician_signoff",
        "canada_ita_submission_package",
        "us_ide_submission_package",
        "informed_consent_package",
        "monitoring_plan",
        "adverse_event_reporting_plan",
        "investigator_package",
        "investigational_labeling",
        "residual_risk_acceptance",
    };
    addMissingSignedArtifacts(decision.clinicalTestingBlockers, evidence, clinicalTestEvidence);
    decision.readyToBeginClinicalTesting = decision.clinicalTestingBlockers.empty();

    return decision;
}

} // namespace dnxr::qms
