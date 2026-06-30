#include "EvidenceBundleVerifier.h"

namespace dnxr {

EvidenceBundleVerificationResult EvidenceBundleVerifier::verify(const std::vector<EvidenceBundleDocument>& docs,
                                                                 const EvidenceRevocationList& rev,
                                                                 const BuildFingerprint& build,
                                                                 const std::string& trust,
                                                                 std::int64_t now) const {
    EvidenceBundleVerificationResult r;
    CryptoSignatureVerifier verifier;
    for (const auto& d : docs) {
        std::string reason;
        if (d.signedBuildHash != build.buildHash) {
            r.invalid.push_back(d.signedDocument.artifactId + ":build_hash_mismatch");
        }
        if (d.signedSourceManifestHash != build.sourceManifestHash) {
            r.invalid.push_back(d.signedDocument.artifactId + ":source_manifest_hash_mismatch");
        }
        if (rev.isRevoked(d.signedDocument.signature)) {
            r.invalid.push_back(d.signedDocument.artifactId + ":signature_revoked");
        }
        if (!verifier.verify(d.signedDocument, trust, now, &reason)) {
            r.invalid.push_back(d.signedDocument.artifactId + ":" + reason);
        }
    }
    r.valid = !docs.empty() && r.invalid.empty();
    return r;
}

} // namespace dnxr
