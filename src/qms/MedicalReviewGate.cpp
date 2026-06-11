#include "qms/MedicalReviewGate.h"

namespace dnxr {
MedicalReviewDecision MedicalReviewGate::evaluate(const MedicalReviewEvidence& e) const {
    MedicalReviewDecision d;
    auto need = [&](bool ok, const char* gap){ if(!ok) d.gaps.push_back(gap); };
    need(e.fullBuildPresent, "full build evidence");
    need(e.ctestCompletePresent, "complete CTest evidence");
    need(e.hilP95P99Present, "HIL p95/p99 metrics");
    need(e.riskManagementFilePresent, "risk management file");
    need(e.traceabilityComplete, "complete traceability matrix");
    need(e.softwareSafetyCasePresent, "software safety case");
    need(e.cybersecuritySummaryPresent, "cybersecurity summary");
    need(e.sbomPresent, "SBOM");
    need(e.noOverstatedHumanClaims, "no overstated human-use claims evidence");
    need(e.noElectricalStimulationParams, "no electrical stimulation parameter evidence");
    need(e.noDirectHardwareDispatch, "no direct hardware dispatch evidence");
    need(e.allHumanUseGatesBlocked, "all human-use gates blocked");
    d.allowHumanUse = false;
    if (!e.allHumanUseGatesBlocked || !e.noDirectHardwareDispatch) d.status = MedicalReviewStatus::Blocked;
    else if (!d.gaps.empty()) d.status = MedicalReviewStatus::EvidenceIncomplete;
    else if (!e.independentSignoffPresent) d.status = MedicalReviewStatus::RequiresIndependentSignoff;
    else d.status = MedicalReviewStatus::ReviewCandidate;
    return d;
}
const char* toString(MedicalReviewStatus status) {
    switch(status){case MedicalReviewStatus::Blocked: return "Blocked"; case MedicalReviewStatus::ReviewCandidate: return "ReviewCandidate"; case MedicalReviewStatus::EvidenceIncomplete: return "EvidenceIncomplete"; case MedicalReviewStatus::RequiresIndependentSignoff: return "RequiresIndependentSignoff";} return "Unknown";
}
} // namespace dnxr
