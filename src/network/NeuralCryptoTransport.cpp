#include "NeuralCryptoTransport.h"

#include <cstring>

namespace dnxr {

namespace {
std::uint8_t keyByteAt(const std::string& key, std::size_t idx) {
    if (key.empty()) return 0U;
    return static_cast<std::uint8_t>(key[idx % key.size()]);
}
}

std::vector<std::uint8_t> NeuralCryptoTransport::encrypt(const std::vector<float>& neuralPayload, const std::string& key) const {
    if (!isKeyValid(key) || neuralPayload.empty()) return {};

    std::vector<std::uint8_t> bytes(neuralPayload.size() * sizeof(float));
    std::memcpy(bytes.data(), neuralPayload.data(), bytes.size());
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        bytes[i] = static_cast<std::uint8_t>(bytes[i] ^ keyByteAt(key, i));
    }
    return bytes;
}

std::vector<float> NeuralCryptoTransport::decrypt(const std::vector<std::uint8_t>& cipherPayload, const std::string& key) const {
    if (!isKeyValid(key) || cipherPayload.empty() || cipherPayload.size() % sizeof(float) != 0) return {};

    std::vector<std::uint8_t> plain = cipherPayload;
    for (std::size_t i = 0; i < plain.size(); ++i) {
        plain[i] = static_cast<std::uint8_t>(plain[i] ^ keyByteAt(key, i));
    }

    std::vector<float> out(plain.size() / sizeof(float));
    std::memcpy(out.data(), plain.data(), plain.size());
    return out;
}

bool NeuralCryptoTransport::isKeyValid(const std::string& key) const {
    return key.size() >= 8;
}

} // namespace dnxr
