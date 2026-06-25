#pragma once
#include "core/RuntimeDeploymentMode.h"
#include <string>
namespace dnxr {
struct HardwareBenchSessionManifest {
    std::string sessionId;
    RuntimeDeploymentMode deploymentMode=RuntimeDeploymentMode::HardwareBenchNoHuman;
    bool humanSubjectPresent=false;
    bool humanUseEnabled=false;
    std::string targetType="DUMMY_LOAD";
    std::string targetId;
    std::string hardwareSerial;
    std::string firmwareVersion;
    bool safetyKernelEnabled=false;
    bool emergencyStopVerified=false;
    bool physicalEmergencyStopVerified=false;
    bool traceRecordingEnabled=false;
    bool qmsSignatureValid=false;
    bool activeStimulationEnabled=false;
    float maxOutputCeiling=0.0f;
    std::string qmsSignaturePath;
    std::string trustStorePath;
    std::string revocationListPath;
    std::string operatorId;
    std::string protocolId;
};
class HardwareBenchSession { public: static HardwareBenchSessionManifest dryRunManifest(); static HardwareBenchSessionManifest loadManifest(const std::string& path); static bool isAccepted(const HardwareBenchSessionManifest& manifest, std::string* reason=nullptr); };
}
