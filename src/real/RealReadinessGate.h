#pragma once
#include "RealModePolicy.h"
#include <string>
namespace dnxr {
struct RealReadinessEvidence {
    RuntimeDeploymentMode deploymentMode = RuntimeDeploymentMode::SimulationOnly;
    bool humanUseEnabled = false;
    bool humanSubjectPresent = false;
    bool hardwareSelfTestPassed = false;
    bool safetyKernelOnline = false;
    bool emergencyStopReady = false;
    bool traceRecorderOnline = false;
    bool qmsSignatureValid = false;
    bool authorizedClinicalTrial = false;
    bool activeStimulationRequested = false;
    std::string targetType = "NONE";
};
struct RealReadinessDecision { bool allowed = false; std::string reason; };
class RealReadinessGate { public: RealReadinessDecision evaluate(const RealReadinessEvidence& evidence) const; bool allowsHardwareBenchNoHuman(const RealReadinessEvidence& evidence) const; };
}
