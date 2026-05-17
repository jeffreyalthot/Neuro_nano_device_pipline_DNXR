#include "OpenNeuralInteropProtocol.h"
#include <cmath>

namespace dnxr {

std::vector<float> OpenNeuralInteropProtocol::encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const {
    std::vector<float> packet;
    // [0]=protocol version, [1]=vendor marker, [2]=payload length
    packet.push_back(kProtocolVersion);
    packet.push_back(static_cast<float>(vendorId.size()) / 32.0F);
    packet.push_back(static_cast<float>(payload.size()));
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

bool OpenNeuralInteropProtocol::validateStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId) const {
    if (packet.size() < 3U) return false;
    if (!std::isfinite(packet[0]) || !std::isfinite(packet[1]) || !std::isfinite(packet[2])) return false;
    if (std::fabs(packet[0] - kProtocolVersion) > 1e-4F) return false;
    const float expectedVendorMarker = static_cast<float>(expectedVendorId.size()) / 32.0F;
    if (std::fabs(packet[1] - expectedVendorMarker) > 1e-4F) return false;
    const std::size_t declaredPayloadSize = static_cast<std::size_t>(packet[2]);
    return (packet.size() - 3U) == declaredPayloadSize;
}

std::string OpenNeuralInteropProtocol::metadataTemplate() const {
    return "dnxr-open-neural-packet-v1:{version,vendor,payload_length,payload,calibration,safety,vendor,thermal,energy}";
}

} // namespace dnxr
