#pragma once
#include "../core/RuntimeDeploymentMode.h"
#include <string>
namespace dnxr {
struct HardwareBenchSessionManifest { std::string sessionId; RuntimeDeploymentMode deploymentMode=RuntimeDeploymentMode::HardwareBenchNoHuman; bool humanSubjectPresent=false; std::string targetType="DUMMY_LOAD"; bool emergencyStopVerified=false; bool traceRecordingEnabled=false; bool safetyKernelEnabled=false; bool activeStimulationEnabled=false; float maxOutputCeiling=0.0f; };
class HardwareBenchSession { public: static HardwareBenchSessionManifest dryRunManifest(); static HardwareBenchSessionManifest loadManifest(const std::string& path); static bool isAccepted(const HardwareBenchSessionManifest& manifest, std::string* reason=nullptr); };
}
