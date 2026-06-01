#pragma once

#include "SecureSession.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class NeuralCryptoTransport {
public:
    std::vector<std::uint8_t> encrypt(const std::vector<float>& neuralPayload, const std::string& key) const;
    std::vector<float> decrypt(const std::vector<std::uint8_t>& cipherPayload, const std::string& key) const;
    bool isKeyValid(const std::string& key) const;

    SecureSession createSession(
        const std::string& identity,
        const std::string& keyMaterial,
        std::int64_t ttlMs = 60000) const;
    NeuralSecurePacket seal(const std::vector<float>& neuralPayload, SecureSession& session) const;
    bool open(const NeuralSecurePacket& packet, SecureSession& session, std::vector<float>& neuralPayload) const;
};

} // namespace dnxr
