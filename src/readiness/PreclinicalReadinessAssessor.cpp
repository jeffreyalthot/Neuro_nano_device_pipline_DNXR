#include "PreclinicalReadinessAssessor.h"

namespace dnxr {

PreclinicalReadinessReport PreclinicalReadinessAssessor::assess(const PreclinicalEvidenceChecklist& c) const {
    PreclinicalReadinessReport report;
    if (!c.fullDiveSessionValidated) report.gaps.push_back("full-dive session validation");
    if (!c.hilFaultInjectionPassed) report.gaps.push_back("HIL fault injection evidence");
    if (!c.latencyBudgetPassed) report.gaps.push_back("latency budget evidence");
    if (!c.emergencyStopPassed) report.gaps.push_back("emergency stop evidence");
    if (!c.recoverySequencePassed) report.gaps.push_back("recovery sequence evidence");
    if (!c.riskControlsVerified) report.gaps.push_back("risk control verification");
    report.ready = report.gaps.empty();
    return report;
}

PreclinicalReadinessReport PreclinicalReadinessAssessor::assess(const PreclinicalEvidenceChecklist& c, const EvidenceRepository& evidence) const {
    PreclinicalReadinessReport report = assess(c);
    if (!evidence.hasSignedEvidenceOfType("physical_hil_session")) {
        report.gaps.push_back("signed physical HIL session report");
    }
    if (!evidence.hasSignedEvidenceOfType("hil_latency_report")) {
        report.gaps.push_back("signed HIL latency report");
    }
    if (!evidence.hasSignedEvidenceOfType("risk_control_verification")) {
        report.gaps.push_back("signed risk-control verification record");
    }
    if (!evidence.hasSignedEvidenceOfType("runtime_10k_full")) {
        report.gaps.push_back("signed full 10k runtime execution evidence");
    }
    report.ready = report.gaps.empty();
    return report;
}

} // namespace dnxr
