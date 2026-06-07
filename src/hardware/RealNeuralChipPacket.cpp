#include "RealNeuralChipPacket.h"

#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {
std::string deterministicPacketHash(const std::string& value) {
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
std::string computePacketHash(const RealNeuralChipPacket& p){
    std::ostringstream oss;
    oss << p.sessionId << ':' << p.previousCommandHash << ':' << p.packetId << ':' << p.modalityMask;
    for(float v:p.payload) oss << ':' << v;
    return deterministicPacketHash(oss.str());
}
} // namespace dnxr
