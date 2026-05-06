#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct MeshPacket {
    std::string source;
    std::string destination;
    std::vector<float> payload;
};

class NeuralMeshProtocol {
public:
    /// Encodes a neural signal payload into a routable packet.
    MeshPacket encode(const std::string& src, const std::string& dst, const std::vector<float>& signal) const;
    /// Decodes the payload from a received packet.
    std::vector<float> decode(const MeshPacket& packet) const;
};

} // namespace dnxr
