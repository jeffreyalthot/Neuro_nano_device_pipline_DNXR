#include "OpenNeuralInteropProtocol.h"

#include <cmath>

namespace dnxr {

float OpenNeuralInteropProtocol::computeChecksum(const std::vector<float>& payload) const {
    float acc = 0.0f;
    for (std::size_t i = 0; i < payload.size(); ++i) {
        acc += payload[i] * static_cast<float>((i % 7U) + 1U);
    }
    return acc;
}

std::vector<float> OpenNeuralInteropProtocol::encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const {
    std::vector<float> packet;
    // [0]=protocol version, [1]=vendor marker, [2]=payload length, [3]=checksum
    packet.push_back(kProtocolVersion);
    packet.push_back(static_cast<float>(vendorId.size()) / 32.0F);
    packet.push_back(static_cast<float>(payload.size()));
    packet.push_back(computeChecksum(payload));
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

bool OpenNeuralInteropProtocol::validateStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId) const {
    OpenNeuralPacket decoded;
    return decodeStandardPacket(packet, expectedVendorId, decoded);
}

bool OpenNeuralInteropProtocol::decodeStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId, OpenNeuralPacket& out) const {
    if (packet.size() < 4U) return false;
    if (!std::isfinite(packet[0]) || !std::isfinite(packet[1]) || !std::isfinite(packet[2]) || !std::isfinite(packet[3])) return false;
    if (std::fabs(packet[0] - kProtocolVersion) > 1e-4F) return false;
    const float expectedVendorMarker = static_cast<float>(expectedVendorId.size()) / 32.0F;
    if (std::fabs(packet[1] - expectedVendorMarker) > 1e-4F) return false;

    const float rawSize = packet[2];
    if (rawSize < 0.0f || std::fabs(rawSize - std::round(rawSize)) > 1e-4F) return false;
    const std::size_t declaredPayloadSize = static_cast<std::size_t>(rawSize);
    if ((packet.size() - 4U) != declaredPayloadSize) return false;

    std::vector<float> payload(packet.begin() + 4, packet.end());
    const float checksum = computeChecksum(payload);
    if (std::fabs(packet[3] - checksum) > 1e-3F) return false;

    out.version = packet[0];
    out.vendorMarker = packet[1];
    out.payload = std::move(payload);
    out.checksum = packet[3];
    return true;
}

std::string OpenNeuralInteropProtocol::metadataTemplate() const {
    return "dnxr-open-neural-packet-v1_1:{version,vendor,payload_length,checksum,payload,calibration,safety,thermal,energy}";
}

} // namespace dnxr
