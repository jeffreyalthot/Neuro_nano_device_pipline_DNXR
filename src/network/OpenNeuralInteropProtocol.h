#pragma once

#include <string>
#include <vector>

namespace dnxr {

class OpenNeuralInteropProtocol {
public:
    static constexpr float kProtocolVersion = 1.0F;

    std::vector<float> encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const;
    bool validateStandardPacket(const std::vector<float>& packet, const std::string& expectedVendorId) const;
    std::string metadataTemplate() const;
};

} // namespace dnxr
