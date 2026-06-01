#pragma once

#include "SecureSession.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

#ifndef DNXR_SIMULATION_ONLY
#define DNXR_SIMULATION_ONLY 1
#endif

#if !DNXR_SIMULATION_ONLY
#error Real crypto backend required for non-simulation builds
#endif

class SimulationCryptoTransport {
public:
    static constexpr bool kCryptographicallySecure = false;
    static constexpr bool kSimulationOnly = true;

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
