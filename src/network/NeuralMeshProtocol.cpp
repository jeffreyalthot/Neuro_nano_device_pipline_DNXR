#include "NeuralMeshProtocol.h"

namespace dnxr {

MeshPacket NeuralMeshProtocol::encode(
    const std::string& src,
    const std::string& dst,
    const std::vector<float>& signal) const {
    // This function packages source, destination, and payload into a routable mesh packet.
    return {src, dst, signal};
}

std::vector<float> NeuralMeshProtocol::decode(const MeshPacket& packet) const {
    // This function extracts the payload from a received mesh packet for local processing.
    return packet.payload;
}

} // namespace dnxr
