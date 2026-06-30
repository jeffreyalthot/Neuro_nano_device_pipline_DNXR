#pragma once

#include "BuildFingerprint.h"
#include "CryptoSignatureVerifier.h"
#include "EvidenceRevocationList.h"

#include <string>
#include <vector>

namespace dnxr {

struct EvidenceBundleDocument {
    SignedEvidenceDocument signedDocument;
    std::string signedBuildHash;
    std::string signedSourceManifestHash;
    std::string patientId;
    std::string sessionId;
    std::string protocolId;
};

struct EvidenceBundleVerificationResult {
    bool valid = false;
    std::vector<std::string> invalid;
};

class EvidenceBundleVerifier {
public:
    EvidenceBundleVerificationResult verify(const std::vector<EvidenceBundleDocument>& docs,
                                            const EvidenceRevocationList& revocations,
                                            const BuildFingerprint& build,
                                            const std::string& trustAnchor,
                                            std::int64_t now) const;
};

} // namespace dnxr
