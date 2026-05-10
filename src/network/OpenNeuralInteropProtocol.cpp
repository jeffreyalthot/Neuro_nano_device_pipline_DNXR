#include "OpenNeuralInteropProtocol.h"

namespace dnxr {

std::vector<float> OpenNeuralInteropProtocol::encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const {
    std::vector<float> packet;
    packet.push_back(static_cast<float>(vendorId.size()) / 32.0F); // vendor metadata marker
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

std::string OpenNeuralInteropProtocol::metadataTemplate() const {
    return "dnxr-open-neural-packet-v1:{calibration,safety,vendor,thermal,energy}";
}

} // namespace dnxr
