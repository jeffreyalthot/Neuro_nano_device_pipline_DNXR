#pragma once

#include "ReplayProtectionWindow.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct SecureSessionKeys {
    std::string transportKey;
    std::string auditKey;
    std::string identityKey;
};

struct NeuralSecurePacket {
    std::string sessionId;
    std::uint64_t counter = 0;
    std::string nonce;
    std::vector<std::uint8_t> cipherPayload;
    std::string signature;
    std::int64_t expiresAtMs = 0;
};

class SecureSession {
public:
    SecureSession(
        std::string sessionId,
        SecureSessionKeys keys,
        std::int64_t ttlMs = 60000,
        std::size_t replayWindowSize = 128U);

    NeuralSecurePacket seal(const std::vector<float>& neuralPayload);
    bool open(const NeuralSecurePacket& packet, std::vector<float>& neuralPayload);

    const std::string& sessionId() const;
    const SecureSessionKeys& keys() const;
    std::uint64_t nextCounter() const;
    bool isExpired(std::int64_t nowMs) const;
    bool isValid() const;
    std::int64_t expiresAtMs() const;
    std::uint64_t highestAcceptedCounter() const;

private:
    std::string makeNonce(std::uint64_t counter) const;
    std::vector<std::uint8_t> encodeFloats(const std::vector<float>& payload) const;
    std::vector<float> decodeFloats(const std::vector<std::uint8_t>& payload) const;
    std::vector<std::uint8_t> applyStreamCipher(
        const std::vector<std::uint8_t>& payload,
        const std::string& nonce,
        std::uint64_t counter) const;
    std::int64_t nowMs() const;

    std::string sessionId_;
    SecureSessionKeys keys_;
    std::int64_t expiresAtMs_ = 0;
    std::uint64_t nextCounter_ = 1;
    ReplayProtectionWindow replayWindow_;
};

} // namespace dnxr
