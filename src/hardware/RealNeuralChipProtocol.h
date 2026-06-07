#pragma once
#include "INeuralChipTransport.h"
#include "../hardware/NeuralChipHAL.h"
namespace dnxr { class RealNeuralChipProtocol { public: RealNeuralChipPacket buildPacket(const SafeNeuralStimulusCommandFixed& command, const std::string& sessionId, const std::string& previousHash) const; bool validateLockedClinicalPrerequisites(bool clinicalGate, bool attestation, bool signedEvidence, bool physicalEmergencyStop) const; }; }
