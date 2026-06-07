#pragma once
#include "../hardware/RealNeuralChipPacket.h"
#include <string>
namespace dnxr { class ClinicalPacketSigner { public: explicit ClinicalPacketSigner(std::string sessionKeyHash="simulation_session_key"); std::string sign(const RealNeuralChipPacket& packet) const; std::string algorithm() const; private: std::string sessionKeyHash_; }; }
