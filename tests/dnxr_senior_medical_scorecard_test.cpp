#include "readiness/ReadinessScorecard.h"

#include <cassert>
#include <string>

static dnxr::EvidenceArtifact artifact(const std::string& type) {
    return {"EV-" + type, "qms/generated/" + type + ".md", type, "REQ-" + type, "H-001", dnxr::EvidenceReviewState::Signed, "physician developer", "2026-06-30"};
}

int main() {
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

    const auto approvalRows = scorecard.seniorMedicalDomainRatings();
    assert(approvalRows.size() == 10U);
    for (const auto& row : approvalRows) {
        if (row.domain == "Readiness usage humain") {
            assert(row.score == 10.0);
            assert(row.blocksHumanUse);
            assert(row.status == dnxr::ReadinessDomainStatus::ApprovalRequestReadyHumanUseLocked);
        } else {
            assert(row.score == 10.0);
            assert(!row.blocksHumanUse);
            assert(row.status == dnxr::ReadinessDomainStatus::LockedComplete);
        }
    }

    dnxr::EvidenceRepository repo;
    for (const std::string type : {"physical_hil_session", "physical_hil_raw_logs", "phantom_qualification_report", "target_hardware_latency_report", "risk_control_verification", "runtime_10k_full", "cybersecurity_executed", "clinical_protocol_package", "reb_irb_approval", "physician_signoff", "canada_ita_submission_package", "us_ide_submission_package", "informed_consent_package", "monitoring_plan", "adverse_event_reporting_plan", "investigator_package", "investigational_labeling", "residual_risk_acceptance"}) {
        repo.add(artifact(type));
    }

    const auto humanReadyRows = scorecard.seniorMedicalDomainRatings(repo);
    for (const auto& row : humanReadyRows) {
        if (row.domain == "Readiness usage humain") {
            assert(row.score == 10.0);
            assert(!row.blocksHumanUse);
            assert(row.status == dnxr::ReadinessDomainStatus::HumanUseReady);
        }
    }

    const std::string approvalTable = scorecard.seniorMedicalMarkdownTable();
    assert(approvalTable.find("Readiness usage humain | 10/10 | approval_request_ready_human_use_locked") != std::string::npos);
    const std::string humanReadyTable = scorecard.seniorMedicalMarkdownTable(repo);
    assert(humanReadyTable.find("Readiness usage humain | 10/10 | human_use_ready") != std::string::npos);
    assert(humanReadyTable.find("Cryptographie production | 10/10") != std::string::npos);
    return 0;
}
