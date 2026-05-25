#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class NeuralCryptoTransport {
public:
    std::vector<std::uint8_t> encrypt(const std::vector<float>& neuralPayload, const std::string& key) const;
    std::vector<float> decrypt(const std::vector<std::uint8_t>& cipherPayload, const std::string& key) const;
    bool isKeyValid(const std::string& key) const;
};

} // namespace dnxr
