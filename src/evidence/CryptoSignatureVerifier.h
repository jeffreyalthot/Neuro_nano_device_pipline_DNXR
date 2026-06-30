#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

struct SignedEvidenceDocument {
    std::string artifactId;
    std::string payloadHash;
    std::string signature;
    std::string signerId;
    std::string signerRole;
    std::string keyId;
    std::int64_t signedAtUnixSeconds = 0;
    std::int64_t expiresAtUnixSeconds = 0;
};

class CryptoSignatureVerifier {
public:
    static std::string digestHex(const std::string& input);
    static std::string canonicalPayload(const SignedEvidenceDocument& document);
    bool verify(const SignedEvidenceDocument& document,
                const std::string& trustAnchor,
                std::int64_t nowUnixSeconds,
                std::string* reason = nullptr) const;
};

} // namespace dnxr
