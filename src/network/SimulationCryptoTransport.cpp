#include "SimulationCryptoTransport.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace dnxr {

namespace {
std::uint8_t keyByteAt(const std::string& key, std::size_t idx) {
    if (key.empty()) return 0U;
    return static_cast<std::uint8_t>(key[idx % key.size()]);
}

std::uint64_t fnv1a64(const std::string& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::string hexDigest(const std::string& prefix, const std::string& payload) {
    std::ostringstream out;
    out << prefix << std::hex << std::setw(16) << std::setfill('0') << fnv1a64(payload);
    return out.str();
}
} // namespace

std::vector<std::uint8_t> SimulationCryptoTransport::encrypt(const std::vector<float>& neuralPayload, const std::string& key) const {
    if (!isKeyValid(key) || neuralPayload.empty()) return {};

    std::vector<std::uint8_t> bytes(neuralPayload.size() * sizeof(float));
    std::memcpy(bytes.data(), neuralPayload.data(), bytes.size());
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        bytes[i] = static_cast<std::uint8_t>(bytes[i] ^ keyByteAt(key, i));
    }
    return bytes;
}

std::vector<float> SimulationCryptoTransport::decrypt(const std::vector<std::uint8_t>& cipherPayload, const std::string& key) const {
    if (!isKeyValid(key) || cipherPayload.empty() || cipherPayload.size() % sizeof(float) != 0) return {};

    std::vector<std::uint8_t> plain = cipherPayload;
    for (std::size_t i = 0; i < plain.size(); ++i) {
        plain[i] = static_cast<std::uint8_t>(plain[i] ^ keyByteAt(key, i));
    }

    std::vector<float> out(plain.size() / sizeof(float));
    std::memcpy(out.data(), plain.data(), plain.size());
    return out;
}

bool SimulationCryptoTransport::isKeyValid(const std::string& key) const {
    return key.size() >= 8;
}

SecureSession SimulationCryptoTransport::createSession(
    const std::string& identity,
    const std::string& keyMaterial,
    std::int64_t ttlMs) const {

    SecureSessionKeys keys;
    if (!isKeyValid(keyMaterial)) {
        return SecureSession("invalid_session", keys, ttlMs);
    }

    keys.transportKey = hexDigest("transport_", keyMaterial + "|transport");
    keys.auditKey = hexDigest("audit_", keyMaterial + "|audit");
    keys.identityKey = hexDigest("identity_", keyMaterial + "|" + identity + "|identity");

    const std::string sessionId = hexDigest("session_", identity + "|" + keyMaterial + "|simulation_secure_session");
    return SecureSession(sessionId, keys, ttlMs);
}

NeuralSecurePacket SimulationCryptoTransport::seal(const std::vector<float>& neuralPayload, SecureSession& session) const {
    return session.seal(neuralPayload);
}

bool SimulationCryptoTransport::open(
    const NeuralSecurePacket& packet,
    SecureSession& session,
    std::vector<float>& neuralPayload) const {
    return session.open(packet, neuralPayload);
}

} // namespace dnxr
