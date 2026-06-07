#pragma once
#include "ClinicalPacketSigner.h"
namespace dnxr { class ClinicalPacketVerifier { public: bool verify(const RealNeuralChipPacket& packet, const ClinicalPacketSigner& signer, bool simulationAllowed) const; }; }
