#include "SecureSession.h"

#include "PacketSigner.h"

#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

std::uint64_t fnv1a64(const std::string& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::uint8_t streamByte(const std::string& material, std::size_t index) {
    const auto digest = fnv1a64(material + "|" + std::to_string(index / 8U));
    return static_cast<std::uint8_t>((digest >> ((index % 8U) * 8U)) & 0xffU);
}

} // namespace

SecureSession::SecureSession(
    std::string sessionId,
    SecureSessionKeys keys,
    std::int64_t ttlMs,
    std::size_t replayWindowSize)
    : sessionId_(std::move(sessionId)),
      keys_(std::move(keys)),
      expiresAtMs_(nowMs() + (ttlMs <= 0 ? 60000 : ttlMs)),
      replayWindow_(replayWindowSize) {}

NeuralSecurePacket SecureSession::seal(const std::vector<float>& neuralPayload) {
    NeuralSecurePacket packet;
    if (!isValid() || neuralPayload.empty()) {
        return packet;
    }

    packet.sessionId = sessionId_;
    packet.counter = nextCounter_++;
    packet.nonce = makeNonce(packet.counter);
    packet.expiresAtMs = expiresAtMs_;
    packet.cipherPayload = applyStreamCipher(encodeFloats(neuralPayload), packet.nonce, packet.counter);
    packet.signature = PacketSigner::sign(
        packet.sessionId,
        packet.counter,
        packet.nonce,
        packet.cipherPayload,
        keys_.auditKey);
    return packet;
}

bool SecureSession::open(const NeuralSecurePacket& packet, std::vector<float>& neuralPayload) {
    neuralPayload.clear();

    if (!isValid() || packet.sessionId != sessionId_ || packet.counter == 0U || packet.cipherPayload.empty()) {
        return false;
    }

    const auto currentMs = nowMs();
    if (isExpired(currentMs) || packet.expiresAtMs <= currentMs) {
        return false;
    }

    if (!PacketSigner::verify(
            packet.sessionId,
            packet.counter,
            packet.nonce,
            packet.cipherPayload,
            packet.signature,
            keys_.auditKey)) {
        return false;
    }

    if (!replayWindow_.accept(packet.counter)) {
        return false;
    }

    const auto plainBytes = applyStreamCipher(packet.cipherPayload, packet.nonce, packet.counter);
    if (plainBytes.empty() || plainBytes.size() % sizeof(float) != 0U) {
        return false;
    }

    neuralPayload = decodeFloats(plainBytes);
    return !neuralPayload.empty();
}

const std::string& SecureSession::sessionId() const {
    return sessionId_;
}

const SecureSessionKeys& SecureSession::keys() const {
    return keys_;
}

std::uint64_t SecureSession::nextCounter() const {
    return nextCounter_;
}

bool SecureSession::isExpired(std::int64_t nowMsValue) const {
    return nowMsValue > expiresAtMs_;
}

bool SecureSession::isValid() const {
    return !sessionId_.empty() &&
           sessionId_ != "invalid_session" &&
           !keys_.transportKey.empty() &&
           !keys_.auditKey.empty() &&
           !keys_.identityKey.empty();
}

std::int64_t SecureSession::expiresAtMs() const {
    return expiresAtMs_;
}

std::uint64_t SecureSession::highestAcceptedCounter() const {
    return replayWindow_.highestAcceptedCounter();
}

std::string SecureSession::makeNonce(std::uint64_t counter) const {
    std::ostringstream payload;
    payload << sessionId_ << '|' << keys_.identityKey << '|' << counter << '|' << expiresAtMs_;
    const auto digest = fnv1a64(payload.str());

    std::ostringstream out;
    out << "nonce_" << std::hex << std::setw(16) << std::setfill('0') << digest;
    return out.str();
}

std::vector<std::uint8_t> SecureSession::encodeFloats(const std::vector<float>& payload) const {
    std::vector<std::uint8_t> bytes(payload.size() * sizeof(float));
    std::memcpy(bytes.data(), payload.data(), bytes.size());
    return bytes;
}

std::vector<float> SecureSession::decodeFloats(const std::vector<std::uint8_t>& payload) const {
    if (payload.empty() || payload.size() % sizeof(float) != 0U) {
        return {};
    }

    std::vector<float> out(payload.size() / sizeof(float));
    std::memcpy(out.data(), payload.data(), payload.size());
    return out;
}

std::vector<std::uint8_t> SecureSession::applyStreamCipher(
    const std::vector<std::uint8_t>& payload,
    const std::string& nonce,
    std::uint64_t counter) const {

    std::vector<std::uint8_t> out = payload;
    const std::string material = keys_.transportKey + '|' + sessionId_ + '|' + nonce + '|' + std::to_string(counter);
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<std::uint8_t>(out[i] ^ streamByte(material, i));
    }
    return out;
}

std::int64_t SecureSession::nowMs() const {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

} // namespace dnxr
