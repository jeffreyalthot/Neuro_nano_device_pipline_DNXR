#pragma once
#include <string>
#include <vector>
namespace dnxr { struct NeuralDeviceHandshakeEvidence { std::string deviceCertificate, firmwareHash, hardwareRevision, calibrationCompatibility, signedSessionNonce; std::vector<std::string> supportedModalities; int maxChannelCount=0; bool emergencyCutoffCapability=false; float latencyCapabilityMs=1000; }; struct NeuralDeviceHandshakeResult { bool accepted=false; std::string reason="not_evaluated"; }; class NeuralDeviceHandshake { public: NeuralDeviceHandshakeResult validate(const NeuralDeviceHandshakeEvidence& e) const; }; }
