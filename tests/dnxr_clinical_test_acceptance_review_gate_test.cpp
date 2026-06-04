#include "../src/qms/ClinicalTestAcceptanceReviewGate.h"

#include <cassert>
#include <string>

static dnxr::ReadinessScorecard completeScorecard() {
    dnxr::ReadinessScorecard scorecard;
    scorecard.fullDiveCapabilityEvidence = true;
    scorecard.integratedSimulationEvidence = true;
    scorecard.hilEvidenceComplete = true;
    scorecard.riskManagementComplete = true;
    scorecard.traceabilityComplete = true;
    scorecard.cybersecurityDocsComplete = true;
    scorecard.verificationValidationComplete = true;
    scorecard.qmsDocsComplete = true;
    scorecard.physicianReviewPackageComplete = true;
    scorecard.canadaUsReadinessDocsComplete = true;
    scorecard.protocolReadinessPackageComplete = true;
    return scorecard;
}

static dnxr::EvidenceArtifact artifact(const std::string& type, dnxr::EvidenceReviewState state) {
    return {"EV-" + type, "qms/generated/" + type + ".md", type, "REQ-" + type, "H-001", state, "named reviewer", "2026-06-04"};
}

int main() {
    dnxr::qms::ClinicalTestAcceptanceReviewGate gate;
    dnxr::EvidenceRepository repo;
    const auto scorecard = completeScorecard();

    auto decision = gate.evaluate(scorecard, repo);
    assert(!decision.readyForExpertAcceptanceReview);
    assert(!decision.readyToBeginClinicalTesting);

    for (const std::string type : {"software_hil_session", "runtime_10k_full", "cybersecurity_package", "qms_signoff_packets", "clinical_external_blockers"}) {
        repo.add(artifact(type, dnxr::EvidenceReviewState::Reviewed));
    }

    decision = gate.evaluate(scorecard, repo);
    assert(decision.readyForExpertAcceptanceReview);
    assert(!decision.readyToBeginClinicalTesting);

    for (const std::string type : {"physical_hil_session", "physical_hil_raw_logs", "phantom_qualification_report", "target_hardware_latency_report", "risk_control_verification", "runtime_10k_full", "cybersecurity_executed", "clinical_protocol_package", "reb_irb_approval", "physician_signoff", "canada_ita_submission_package", "us_ide_submission_package", "informed_consent_package", "monitoring_plan", "adverse_event_reporting_plan", "investigator_package", "investigational_labeling", "residual_risk_acceptance"}) {
        repo.add(artifact(type, dnxr::EvidenceReviewState::Signed));
    }

    decision = gate.evaluate(scorecard, repo);
    assert(decision.readyForExpertAcceptanceReview);
    assert(decision.readyToBeginClinicalTesting);
    return 0;
}
