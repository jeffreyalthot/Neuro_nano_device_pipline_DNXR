#pragma once
#include <string>
#include <vector>
namespace dnxr { class EvidenceRevocationList { public: std::vector<std::string> revokedSignatures; bool isRevoked(const std::string& signature) const; }; }
