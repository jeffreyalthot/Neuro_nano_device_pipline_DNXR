#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct DeviceIdentityEvidence { std::string deviceId; std::string certificate; std::string firmwareVersion; std::string firmwareSignature; std::string calibrationId; bool revoked=false; };
struct DeviceAttestationDecision { bool trusted=false; std::vector<std::string> blockers; };
class DeviceIdentityAttestation { public: DeviceAttestationDecision verify(const DeviceIdentityEvidence& evidence) const; };
}
