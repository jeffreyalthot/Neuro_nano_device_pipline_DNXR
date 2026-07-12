#include "OpenSSLNeuralPacketProtector.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace dnxr {
namespace {
constexpr std::uint8_t kMagic[] = {'D','N','X','R','-','A','E','A','D','1'};
constexpr std::size_t kNonceSize = 12;
constexpr std::size_t kTagSize = 16;
constexpr std::size_t kKeySize = 32;

using CipherContextPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;

std::array<std::uint8_t, kKeySize> deriveKey(const std::string& keyMaterial) {
    if (keyMaterial.size() < 16) {
        throw std::invalid_argument("REAL_PACKET_PROTECTOR_KEY_MATERIAL_TOO_SHORT");
    }
    std::array<std::uint8_t, kKeySize> key{};
    SHA256(reinterpret_cast<const unsigned char*>(keyMaterial.data()), keyMaterial.size(), key.data());
    return key;
}

std::vector<std::uint8_t> encodeFloats(const std::vector<float>& payload) {
    std::vector<std::uint8_t> bytes(payload.size() * sizeof(float));
    if (!payload.empty()) {
        std::memcpy(bytes.data(), payload.data(), bytes.size());
    }
    return bytes;
}

std::vector<float> decodeFloats(const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() % sizeof(float) != 0) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_INVALID_PLAINTEXT_LENGTH");
    }
    std::vector<float> payload(bytes.size() / sizeof(float));
    if (!bytes.empty()) {
        std::memcpy(payload.data(), bytes.data(), bytes.size());
    }
    return payload;
}

void append(std::vector<std::uint8_t>& out, const std::uint8_t* first, std::size_t count) {
    out.insert(out.end(), first, first + count);
}
}

std::vector<std::uint8_t> OpenSSLNeuralPacketProtector::protect(
    const std::vector<float>& neuralPayload,
    const std::string& keyMaterial) const {
    const auto key = deriveKey(keyMaterial);
    std::array<std::uint8_t, kNonceSize> nonce{};
    if (RAND_bytes(nonce.data(), static_cast<int>(nonce.size())) != 1) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_NONCE_GENERATION_FAILED");
    }

    const auto plaintext = encodeFloats(neuralPayload);
    std::vector<std::uint8_t> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
    int outLength = 0;
    int finalLength = 0;
    CipherContextPtr ctx(EVP_CIPHER_CTX_new(), &EVP_CIPHER_CTX_free);
    if (!ctx ||
        EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(nonce.size()), nullptr) != 1 ||
        EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr, key.data(), nonce.data()) != 1 ||
        EVP_EncryptUpdate(ctx.get(), ciphertext.data(), &outLength, plaintext.data(), static_cast<int>(plaintext.size())) != 1 ||
        EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + outLength, &finalLength) != 1) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_ENCRYPT_FAILED");
    }
    ciphertext.resize(static_cast<std::size_t>(outLength + finalLength));

    std::array<std::uint8_t, kTagSize> tag{};
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, static_cast<int>(tag.size()), tag.data()) != 1) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_TAG_FAILED");
    }

    std::vector<std::uint8_t> packet;
    packet.reserve(sizeof(kMagic) + nonce.size() + tag.size() + ciphertext.size());
    append(packet, kMagic, sizeof(kMagic));
    append(packet, nonce.data(), nonce.size());
    append(packet, tag.data(), tag.size());
    append(packet, ciphertext.data(), ciphertext.size());
    return packet;
}

std::vector<float> OpenSSLNeuralPacketProtector::unprotect(
    const std::vector<std::uint8_t>& protectedPayload,
    const std::string& keyMaterial) const {
    constexpr std::size_t headerSize = sizeof(kMagic) + kNonceSize + kTagSize;
    if (protectedPayload.size() < headerSize ||
        std::memcmp(protectedPayload.data(), kMagic, sizeof(kMagic)) != 0) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_INVALID_PACKET");
    }

    const auto key = deriveKey(keyMaterial);
    const std::uint8_t* nonce = protectedPayload.data() + sizeof(kMagic);
    const std::uint8_t* tag = nonce + kNonceSize;
    const std::uint8_t* ciphertext = tag + kTagSize;
    const std::size_t ciphertextSize = protectedPayload.size() - headerSize;
    std::vector<std::uint8_t> plaintext(ciphertextSize + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
    int outLength = 0;
    int finalLength = 0;
    CipherContextPtr ctx(EVP_CIPHER_CTX_new(), &EVP_CIPHER_CTX_free);
    if (!ctx ||
        EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(kNonceSize), nullptr) != 1 ||
        EVP_DecryptInit_ex(ctx.get(), nullptr, nullptr, key.data(), nonce) != 1 ||
        EVP_DecryptUpdate(ctx.get(), plaintext.data(), &outLength, ciphertext, static_cast<int>(ciphertextSize)) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_TAG, static_cast<int>(kTagSize), const_cast<std::uint8_t*>(tag)) != 1 ||
        EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + outLength, &finalLength) != 1) {
        throw std::runtime_error("REAL_PACKET_PROTECTOR_AUTHENTICATION_FAILED");
    }
    plaintext.resize(static_cast<std::size_t>(outLength + finalLength));
    return decodeFloats(plaintext);
}

} // namespace dnxr
