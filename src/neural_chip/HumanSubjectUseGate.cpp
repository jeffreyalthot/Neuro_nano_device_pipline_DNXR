#include "neural_chip/HumanSubjectUseGate.h"

namespace dnxr {

HumanSubjectUseDecision HumanSubjectUseGate::evaluate(const HumanSubjectUseEvidence& evidence) const {
    HumanSubjectUseDecision decision;
    decision.allowHumanUse = false;
    decision.allowStimulusDispatch = false;
    decision.allowSimulation = true;
    decision.allowHil = true;

    if (!evidence.benchProfilePresent && !evidence.hilEvidencePresent) {
        decision.status = HumanSubjectUseStatus::BlockedSimulationOnly;
        decision.allowHardwareBench = false;
        decision.reason = "Default release lock: simulation and HIL evaluation only; human-use remains blocked.";
        return decision;
    }

    if (evidence.benchProfilePresent || evidence.hilEvidencePresent) {
        decision.status = HumanSubjectUseStatus::PreclinicalReviewCandidate;
        decision.allowHardwareBench = evidence.benchProfilePresent;
        decision.reason = "Bench/HIL evidence may support medical review, but does not authorize human-subject activity.";
    }

    const bool externalPackagePresent = evidence.regulatoryAuthorizationPresent &&
                                        evidence.ethicsApprovalPresent &&
                                        evidence.physicianSignoffPresent &&
                                        evidence.hardwareBenchSignoffPresent;
    if (!externalPackagePresent) {
        if (decision.status == HumanSubjectUseStatus::BlockedSimulationOnly) {
            decision.status = HumanSubjectUseStatus::RegulatoryAuthorizationRequired;
        }
        decision.reason += " External regulatory, ethics, physician, and hardware approvals are required before any human-subject activity.";
        return decision;
    }

    decision.status = HumanSubjectUseStatus::ExternalAuthorizationPresent;
    decision.allowHardwareBench = true;
    decision.allowHumanUse = false;
    decision.allowStimulusDispatch = false;
    decision.reason = "External authorization evidence is present for review only; software release mode still blocks human use and neural stimulus dispatch.";
    return decision;
}

const char* toString(HumanSubjectUseStatus status) {
    switch (status) {
    case HumanSubjectUseStatus::BlockedSimulationOnly: return "BlockedSimulationOnly";
    case HumanSubjectUseStatus::PreclinicalReviewCandidate: return "PreclinicalReviewCandidate";
    case HumanSubjectUseStatus::BenchHilOnly: return "BenchHilOnly";
    case HumanSubjectUseStatus::RegulatoryAuthorizationRequired: return "RegulatoryAuthorizationRequired";
    case HumanSubjectUseStatus::ExternalAuthorizationPresent: return "ExternalAuthorizationPresent";
    }
    return "Unknown";
}

} // namespace dnxr
