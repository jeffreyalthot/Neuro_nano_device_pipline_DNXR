#include "PacketSigner.h"

#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {

std::uint64_t fnv1a64Bytes(const std::string& header, const std::vector<std::uint8_t>& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : header) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    for (std::uint8_t byte : payload) {
        hash ^= static_cast<std::uint64_t>(byte);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

std::string PacketSigner::sign(
    const std::string& sessionId,
    std::uint64_t counter,
    const std::string& nonce,
    const std::vector<std::uint8_t>& payload,
    const std::string& signingKey) {

    std::ostringstream header;
    header << sessionId << '|' << counter << '|' << nonce << '|' << signingKey << '|';
    const auto digest = fnv1a64Bytes(header.str(), payload);

    std::ostringstream out;
    out << "sig_" << std::hex << std::setw(16) << std::setfill('0') << digest;
    return out.str();
}

bool PacketSigner::verify(
    const std::string& sessionId,
    std::uint64_t counter,
    const std::string& nonce,
    const std::vector<std::uint8_t>& payload,
    const std::string& signature,
    const std::string& signingKey) {

    return signature == sign(sessionId, counter, nonce, payload, signingKey);
}

} // namespace dnxr
