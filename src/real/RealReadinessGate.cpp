#include "RealReadinessGate.h"
namespace dnxr {
static bool allowedTarget(const std::string& t){ return t=="DUMMY_LOAD" || t=="PHANTOM"; }
RealReadinessDecision RealReadinessGate::evaluate(const RealReadinessEvidence& e) const {
    const auto p = RealModePolicy{}.permissionsFor(e.deploymentMode);
    if (!p.realTransportAllowed) return {false,"deployment_mode_does_not_allow_real_transport"};
    if ((e.humanUseEnabled || e.humanSubjectPresent) && !(p.humanSubjectAllowed && e.authorizedClinicalTrial && e.qmsSignatureValid)) return {false,"human_use_requires_authorized_clinical_trial_and_signed_evidence"};
    if (!e.hardwareSelfTestPassed) return {false,"hardware_self_test_required"};
    if (!e.safetyKernelOnline) return {false,"safety_kernel_required"};
    if (!e.emergencyStopReady) return {false,"emergency_stop_required"};
    if (!e.traceRecorderOnline) return {false,"trace_recorder_required"};
    if (!e.qmsSignatureValid) return {false,"qms_signature_required"};
    if (!p.humanSubjectAllowed && !allowedTarget(e.targetType)) return {false,"non_human_target_must_be_dummy_load_or_phantom"};
    if (e.activeStimulationRequested && e.targetType=="DUMMY_LOAD" && !e.dummyLoadAttached) return {false,"active_dummy_requires_attached_dummy_load"};
    if (e.activeStimulationRequested && e.targetType=="PHANTOM" && !e.phantomProfileSigned) return {false,"phantom_profile_signature_required"};
    if (e.activeStimulationRequested && !p.activeStimulationAllowed) return {false,"active_stimulation_not_allowed"};
    return {true,"real_hardware_authorized_for_controlled_mode"};
}
bool RealReadinessGate::allowsHardwareBenchNoHuman(const RealReadinessEvidence& e) const { return evaluate(e).allowed && e.deploymentMode==RuntimeDeploymentMode::HardwareBenchNoHuman && !e.humanUseEnabled && !e.humanSubjectPresent; }
}
