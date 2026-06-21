#include "qms/QmsSignatureVerifier.h"
#include "qms/QmsRevocationList.h"
#include "qms/QmsTrustStore.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
namespace dnxr { QmsSignatureVerifier::QmsSignatureVerifier(std::string root): root_(std::move(root)) {} QmsSignatureVerification QmsSignatureVerifier::verifySignedArtifact(const std::string& relativePath) const { QmsSignatureVerification r; std::ifstream in(std::filesystem::path(root_) / relativePath); r.present = static_cast<bool>(in); if (!in) return r; std::ostringstream ss; ss << in.rdbuf(); const auto text = ss.str(); auto has = [&](const char* token){ return text.find(token) != std::string::npos; }; r.structurallyValid = has("\"signature\"") && has("\"algorithm\"") && has("Ed25519") && has("\"key_id\"") && has("\"value\""); std::string key; auto pos = text.find("\"key_id\""); if (pos != std::string::npos) { auto colon = text.find(':', pos); auto q1 = text.find('"', colon + 1); auto q2 = text.find('"', q1 + 1); if (q1 != std::string::npos && q2 != std::string::npos) key = text.substr(q1 + 1, q2 - q1 - 1); } QmsTrustStore trust(root_); QmsRevocationList revocations(root_); r.trusted = trust.isTrustedKey(key); r.revoked = revocations.isRevoked(key); r.accepted = r.present && r.structurallyValid && r.trusted && !r.revoked; return r; } }
