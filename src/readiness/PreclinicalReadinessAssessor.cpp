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

} // namespace dnxr
