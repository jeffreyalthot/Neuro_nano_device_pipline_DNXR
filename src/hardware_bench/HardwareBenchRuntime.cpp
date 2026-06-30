#include "HardwareBenchRuntime.h"
#include "qms/QmsSignatureVerifier.h"
#include "safety_kernel/SafetyKernel.h"
#include <filesystem>
#include <fstream>
#include <sstream>
namespace dnxr {
namespace {
bool usesDevelopmentOnlyKey(const std::string& relativePath) {
    std::ifstream in(relativePath);
    for (auto probe = std::filesystem::current_path(); !in && probe.has_parent_path(); probe = probe.parent_path()) {
        in.open(probe / relativePath);
        if (probe == probe.parent_path()) break;
    }
    if (!in) return true;
    std::ostringstream ss;
    ss << in.rdbuf();
    const auto text = ss.str();
    return text.find("dnxr-qms-dev-key") != std::string::npos ||
           text.find("development_only") != std::string::npos ||
           text.find("development-placeholder") != std::string::npos;
}
}

HardwareBenchRuntimeResult HardwareBenchRuntime::startDryRun(RealNeuralChipHAL& hal,const HardwareBenchSessionManifest& m){ std::string reason; if(!HardwareBenchSession::isAccepted(m,&reason)) return {false,true,false,reason}; auto st=HardwareBenchSelfTest{}.runDryRunSelfTest(hal,m); RealReadinessEvidence e; e.deploymentMode=m.deploymentMode; e.humanUseEnabled=m.humanUseEnabled; e.humanSubjectPresent=m.humanSubjectPresent; e.hardwareSelfTestPassed=st.passed(); e.safetyKernelOnline=m.safetyKernelEnabled; e.emergencyStopReady=m.emergencyStopVerified && st.emergencyStopAccepted; e.traceRecorderOnline=m.traceRecordingEnabled; e.qmsSignatureValid=m.qmsSignatureValid; e.qmsSignaturePath=m.qmsSignaturePath; e.trustStorePath=m.trustStorePath; e.revocationListPath=m.revocationListPath; e.targetBindingId=m.targetId; e.operatorId=m.operatorId; e.targetType=m.targetType; e.activeStimulationRequested=false; auto d=RealReadinessGate{}.evaluate(e); if(!d.allowed) return {false,true,false,d.reason}; hal.init(m.hardwareSerial.empty()?"hardware_bench_dry_run":m.hardwareSerial); hal.handshake(); hal.selfTest(); hal.getCapabilities(); return {true,true,false,"hardware_bench_dry_run_started_no_active_output"}; }
HardwareBenchRuntimeResult HardwareBenchRuntime::sendActiveToDummyLoad(RealNeuralChipHAL& hal,const HardwareBenchSessionManifest& m,const DummyLoadAdapter& dl,const BoundedStimulusCommand& c){ if(!dl.attached()) return {false,false,false,"dummy_load_required"}; auto r=startDryRun(hal,m); if(!r.started) return r; if(!m.activeStimulationEnabled) return {false,false,false,"active_stimulation_disabled_by_manifest"}; if(usesDevelopmentOnlyKey(m.qmsSignaturePath)) return {false,false,false,"development_qms_key_forbids_active_output"}; auto cmd=c; cmd.dummyLoadAttached=true; cmd.validatedBySafetyKernel=false; cmd.authorizationScope="hardware_bench"; cmd.normalizedCeiling=cmd.maxOutputCeiling; SafetyKernel safety({m.maxOutputCeiling, cmd.maxDeltaPerFrame, cmd.cumulativeBudget}); auto decision=safety.validate(cmd); if(!decision.allowed) return {false,false,false,decision.reason}; cmd.validatedBySafetyKernel=true; bool sent=hal.sendBoundedFrame(cmd); return {sent,false,sent,sent?decision.reason:"hal_rejected_bounded_frame"}; }
}
