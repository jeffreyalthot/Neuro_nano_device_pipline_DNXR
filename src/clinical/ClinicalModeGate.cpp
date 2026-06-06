#include "ClinicalModeGate.h"

namespace dnxr {

ClinicalModeGateDecision ClinicalModeGate::evaluateHumanUseRequest(
    const std::vector<ClinicalEvidenceRecord>& evidence,
    std::int64_t nowUnixSeconds) const {
    ClinicalEvidenceVerifier verifier;
    const auto verification = verifier.verify(evidence, nowUnixSeconds);
    ClinicalModeGateDecision decision;
    for (const auto& missing : verification.missing) {
        decision.blockers.push_back("missing_signed_evidence:" + missing);
    }
    for (const auto& invalid : verification.invalid) {
        decision.blockers.push_back("invalid_or_expired_signed_evidence:" + invalid);
    }
    decision.humanUseAllowed = verification.complete;
    decision.stimulusDispatchAllowed = false;
    if (decision.humanUseAllowed) {
        decision.blockers.push_back("stimulus_dispatch_requires_separate_preclinical_and_clinical_activation_record");
    }
    return decision;
}

bool ClinicalModeGate::canConnectClinicalDevice(const std::vector<ClinicalEvidenceRecord>& evidence,
                                                std::int64_t nowUnixSeconds) const {
    return evaluateHumanUseRequest(evidence, nowUnixSeconds).humanUseAllowed;
}

} // namespace dnxr
