#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class PacketSigner {
public:
    static std::string sign(
        const std::string& sessionId,
        std::uint64_t counter,
        const std::string& nonce,
        const std::vector<std::uint8_t>& payload,
        const std::string& signingKey);

    static bool verify(
        const std::string& sessionId,
        std::uint64_t counter,
        const std::string& nonce,
        const std::vector<std::uint8_t>& payload,
        const std::string& signature,
        const std::string& signingKey);
};

} // namespace dnxr
