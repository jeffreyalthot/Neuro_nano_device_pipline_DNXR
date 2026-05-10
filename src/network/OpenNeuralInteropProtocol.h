#pragma once

#include <string>
#include <vector>

namespace dnxr {

class OpenNeuralInteropProtocol {
public:
    std::vector<float> encodeStandardPacket(const std::vector<float>& payload, const std::string& vendorId) const;
    std::string metadataTemplate() const;
};

} // namespace dnxr
