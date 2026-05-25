#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct OpenNeuralPacket {
    float version = 0.0f;
    float vendorMarker = 0.0f;
    std::vector<float> payload;
    float checksum = 0.0f;
};

class OpenNeuralInteropProtocol {
public:
    static constexpr float kProtocolVersion = 1.1F;

    std::vector<float> encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const;
    bool validateStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId) const;
    bool decodeStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId, OpenNeuralPacket& out) const;
    std::string metadataTemplate() const;

private:
    float computeChecksum(const std::vector<float>& payload) const;
};

} // namespace dnxr
