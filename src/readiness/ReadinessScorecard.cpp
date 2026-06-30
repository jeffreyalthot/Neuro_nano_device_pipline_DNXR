#include "ReadinessScorecard.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {

double average(std::initializer_list<bool> gates) {
    double met = 0.0;
    for (const bool gate : gates) {
        met += gate ? 1.0 : 0.0;
    }
    return gates.size() == 0 ? 0.0 : (met * 10.0 / static_cast<double>(gates.size()));
}

bool hasHumanUseSignedEvidence(const EvidenceRepository& evidence) {
    constexpr std::array<const char*, 18> requiredTypes = {
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
    for (const char* type : requiredTypes) {
        if (!evidence.hasSignedEvidenceOfType(type)) return false;
    }
    return true;
}

ReadinessDomainStatus statusFor(double score, bool humanUseBlocked) {
    if (humanUseBlocked && score >= 10.0) {
        return ReadinessDomainStatus::ApprovalRequestReadyHumanUseLocked;
    }
    if (humanUseBlocked) return ReadinessDomainStatus::Blocked;
    if (score >= 10.0) return ReadinessDomainStatus::LockedComplete;
    if (score >= 8.0) return ReadinessDomainStatus::PreclinicalReady;
    return ReadinessDomainStatus::EvidenceOpen;
}

std::string scoreText(double score) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(score == static_cast<int>(score) ? 0 : 1) << score << "/10";
    return out.str();
}

} // namespace

const char* toString(ReadinessDomainStatus status) {
    switch (status) {
        case ReadinessDomainStatus::Blocked: return "blocked";
        case ReadinessDomainStatus::EvidenceOpen: return "evidence_open";
        case ReadinessDomainStatus::PreclinicalReady: return "preclinical_ready";
        case ReadinessDomainStatus::LockedComplete: return "locked_complete";
        case ReadinessDomainStatus::ApprovalRequestReadyHumanUseLocked: return "approval_request_ready_human_use_locked";
        case ReadinessDomainStatus::HumanUseReady: return "human_use_ready";
    }
    return "unknown";
}

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

std::vector<SeniorMedicalReadinessDomain> ReadinessScorecard::seniorMedicalDomainRatings() const {
    std::vector<SeniorMedicalReadinessDomain> rows = {
        {"Architecture générale", average({fullDiveCapabilityEvidence, integratedSimulationEvidence, traceabilityComplete}), ReadinessDomainStatus::EvidenceOpen, "capability + simulation + traceability", false},
        {"Volume de tests", average({integratedSimulationEvidence, verificationValidationComplete, hilEvidenceComplete}), ReadinessDomainStatus::EvidenceOpen, "simulation + V&V + HIL", false},
        {"Safety simulation", average({integratedSimulationEvidence, riskManagementComplete, verificationValidationComplete}), ReadinessDomainStatus::EvidenceOpen, "risk controls + V&V", false},
        {"Hardware bench scaffold", average({hilEvidenceComplete, fullDiveCapabilityEvidence, verificationValidationComplete}), ReadinessDomainStatus::EvidenceOpen, "HIL/bench evidence", false},
        {"QMS documentation", average({qmsDocsComplete, riskManagementComplete, traceabilityComplete, physicianReviewPackageComplete}), ReadinessDomainStatus::EvidenceOpen, "QMS + risk + traceability + physician package", false},
        {"Preuves signées réelles", average({physicianReviewPackageComplete, protocolReadinessPackageComplete, canadaUsReadinessDocsComplete}), ReadinessDomainStatus::EvidenceOpen, "signed physician/protocol/jurisdiction artifacts", false},
        {"Cryptographie production", average({cybersecurityDocsComplete, protocolReadinessPackageComplete, traceabilityComplete}), ReadinessDomainStatus::EvidenceOpen, "cybersecurity + protocol binding + traceability", false},
        {"Readiness usage humain", average({physicianReviewPackageComplete, protocolReadinessPackageComplete, canadaUsReadinessDocsComplete, cybersecurityDocsComplete, riskManagementComplete, traceabilityComplete, verificationValidationComplete, qmsDocsComplete, hilEvidenceComplete}), ReadinessDomainStatus::Blocked, "dossier de demande medicale pret; usage humain encore verrouille sans autorisation signee", true},
        {"Due diligence technique", average({traceabilityComplete, qmsDocsComplete, cybersecurityDocsComplete, verificationValidationComplete}), ReadinessDomainStatus::EvidenceOpen, "traceability + QMS + cyber + V&V", false},
        {"Maintenabilité code", average({traceabilityComplete, verificationValidationComplete, qmsDocsComplete}), ReadinessDomainStatus::EvidenceOpen, "traceability + tests + QMS maintenance", false},
    };
    for (auto& row : rows) {
        row.status = statusFor(row.score, row.blocksHumanUse);
    }
    return rows;
}

std::vector<SeniorMedicalReadinessDomain> ReadinessScorecard::seniorMedicalDomainRatings(const EvidenceRepository& evidence) const {
    auto rows = seniorMedicalDomainRatings();
    if (!hasHumanUseSignedEvidence(evidence)) {
        return rows;
    }
    for (auto& row : rows) {
        if (row.domain == "Readiness usage humain" && row.score >= 10.0) {
            row.blocksHumanUse = false;
            row.status = ReadinessDomainStatus::HumanUseReady;
            row.evidenceGate = "documents cliniques signes complets; pret usage humain conforme";
        }
    }
    return rows;
}

std::string ReadinessScorecard::seniorMedicalMarkdownTable() const {
    std::ostringstream out;
    out << "| Domaine | Score | Statut | Evidence gate |\n";
    out << "| --- | ---: | --- | --- |\n";
    for (const auto& row : seniorMedicalDomainRatings()) {
        out << "| " << row.domain << " | " << scoreText(row.score) << " | " << toString(row.status)
            << " | " << row.evidenceGate << " |\n";
    }
    return out.str();
}

std::string ReadinessScorecard::seniorMedicalMarkdownTable(const EvidenceRepository& evidence) const {
    std::ostringstream out;
    out << "| Domaine | Score | Statut | Evidence gate |\n";
    out << "| --- | ---: | --- | --- |\n";
    for (const auto& row : seniorMedicalDomainRatings(evidence)) {
        out << "| " << row.domain << " | " << scoreText(row.score) << " | " << toString(row.status)
            << " | " << row.evidenceGate << " |\n";
    }
    return out.str();
}

} // namespace dnxr
