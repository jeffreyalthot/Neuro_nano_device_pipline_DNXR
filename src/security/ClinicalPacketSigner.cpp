#include "ClinicalPacketSigner.h"
#include <functional>
#include <utility>
namespace dnxr { ClinicalPacketSigner::ClinicalPacketSigner(std::string key):sessionKeyHash_(std::move(key)){} std::string ClinicalPacketSigner::algorithm() const { return "DNXR-HASHCHAIN-HMAC-SHA256"; } std::string ClinicalPacketSigner::sign(const RealNeuralChipPacket& p) const { return "sig_" + std::to_string(std::hash<std::string>{}(sessionKeyHash_ + p.packetHash + p.previousPacketHash + std::to_string(p.antiReplayNonce))); } }
