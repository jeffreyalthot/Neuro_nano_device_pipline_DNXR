#include "HumanSubjectSessionController.h"

namespace dnxr {
namespace {
void require(bool condition, const char* blocker, std::vector<std::string>& blockers) {
    if (!condition) {
        blockers.emplace_back(blocker);
    }
}
}

HumanSubjectSessionDecision HumanSubjectSessionController::validateStart(
    const HumanSubjectSessionPrerequisites& p,
    const std::vector<ClinicalEvidenceRecord>& evidence,
    std::int64_t nowUnixSeconds) const {
    HumanSubjectSessionDecision decision;
    ClinicalModeGate gate;
    const auto gateDecision = gate.evaluateHumanUseRequest(evidence, nowUnixSeconds);
    decision.blockers.insert(decision.blockers.end(), gateDecision.blockers.begin(), gateDecision.blockers.end());
    require(p.informedConsentSigned, "informed_consent_missing", decision.blockers);
    require(p.independentPhysicianPresent, "independent_physician_missing", decision.blockers);
    require(p.rebIrbApproved, "reb_irb_approval_missing", decision.blockers);
    require(p.regulatoryAuthorizationPresent, "regulatory_authorization_missing", decision.blockers);
    require(p.emergencyStopVerified, "software_emergency_stop_not_verified", decision.blockers);
    require(p.externalPhysicalStopVerified, "external_physical_stop_not_verified", decision.blockers);
    require(p.continuousMonitoringAvailable, "continuous_monitoring_missing", decision.blockers);
    require(p.communicationChannelAvailable, "human_communication_channel_missing", decision.blockers);
    require(p.signedCalibrationAvailable, "signed_patient_calibration_missing", decision.blockers);
    require(p.postSessionFollowupScheduled, "post_session_followup_missing", decision.blockers);
    decision.mayStart = gateDecision.humanUseAllowed && decision.blockers.empty();
    return decision;
}

} // namespace dnxr
