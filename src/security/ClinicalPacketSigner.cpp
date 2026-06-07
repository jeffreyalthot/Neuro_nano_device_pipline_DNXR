#include "ClinicalPacketSigner.h"
#include <iomanip>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {
std::string deterministicClinicalHash(const std::string& value) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : value) {
        hash ^= c;
        hash *= 1099511628211ULL;
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}
}
ClinicalPacketSigner::ClinicalPacketSigner(std::string key):sessionKeyHash_(std::move(key)){}
std::string ClinicalPacketSigner::algorithm() const { return "DNXR-SIM-FNV1A-HASHCHAIN"; }
std::string ClinicalPacketSigner::sign(const RealNeuralChipPacket& p) const {
    if (sessionKeyHash_.empty()) return {};
    return "sig_sim_" + deterministicClinicalHash(sessionKeyHash_ + p.packetHash + p.previousPacketHash + std::to_string(p.antiReplayNonce));
}
}
