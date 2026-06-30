#include "CryptoSignatureVerifier.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace dnxr {

static std::uint64_t mix(std::uint64_t x) {
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;
    return x;
}

std::string CryptoSignatureVerifier::digestHex(const std::string& input) {
    std::array<std::uint64_t, 4> h{0x6a09e667f3bcc908ULL,
                                   0xbb67ae8584caa73bULL,
                                   0x3c6ef372fe94f82bULL,
                                   0xa54ff53a5f1d36f1ULL};
    for (unsigned char c : input) {
        for (auto& v : h) {
            v = mix(v ^ c ^ (v << 7));
        }
    }

    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for (auto v : h) {
        os << std::setw(16) << v;
    }
    return os.str();
}

std::string CryptoSignatureVerifier::canonicalPayload(const SignedEvidenceDocument& d) {
    return d.artifactId + "|" + d.payloadHash + "|" + d.signerId + "|" + d.signerRole + "|" + d.keyId +
           "|" + std::to_string(d.signedAtUnixSeconds) + "|" + std::to_string(d.expiresAtUnixSeconds);
}

bool CryptoSignatureVerifier::verify(const SignedEvidenceDocument& d,
                                     const std::string& trustAnchor,
                                     std::int64_t now,
                                     std::string* reason) const {
    auto fail = [&](const char* r) {
        if (reason) *reason = r;
        return false;
    };
    if (d.artifactId.empty() || d.payloadHash.size() != 64 || d.signature.size() != 64 || d.signerId.empty() ||
        d.signerRole.empty() || d.keyId.empty()) {
        return fail("signed_document_fields_missing_or_malformed");
    }
    if (d.signedAtUnixSeconds <= 0) return fail("signed_at_required");
    if (d.expiresAtUnixSeconds <= now) return fail("signed_document_expired");
    if (trustAnchor.empty()) return fail("trust_anchor_required");

    const auto expected = digestHex(canonicalPayload(d) + "|" + trustAnchor);
    if (d.signature != expected) return fail("cryptographic_signature_mismatch");
    if (reason) reason->clear();
    return true;
}

} // namespace dnxr
