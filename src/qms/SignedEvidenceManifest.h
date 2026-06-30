#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct EvidenceManifestFile { std::string path; std::string sha256; std::string role; };
struct SignedEvidenceManifest { std::string release="0.6.6"; std::vector<EvidenceManifestFile> files; std::string generatedAt; std::string generator="dnxr-qms"; std::string toJson() const; };
class SignedEvidenceManifestBuilder { public: static std::string sha256File(const std::string& path); SignedEvidenceManifest build(const std::vector<std::pair<std::string,std::string>>& pathAndRole) const; };
}
