#include "../../src/real/RealReadinessGate.h"
#include <cassert>
int main(){ dnxr::RealReadinessEvidence e; e.deploymentMode=dnxr::RuntimeDeploymentMode::HardwareBenchNoHuman; e.humanUseEnabled=true; e.hardwareSelfTestPassed=e.safetyKernelOnline=e.emergencyStopReady=e.traceRecorderOnline=e.qmsSignatureValid=true; e.targetType="DUMMY_LOAD"; assert(!dnxr::RealReadinessGate{}.evaluate(e).allowed); }
