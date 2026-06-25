#pragma once
#include <string>
namespace dnxr { struct QmsSignatureVerification { bool present = false; bool structurallyValid = false; bool trusted = false; bool revoked = false; bool accepted = false; std::string reason; }; class QmsSignatureVerifier { public: explicit QmsSignatureVerifier(std::string root = "."); QmsSignatureVerification verifySignedArtifact(const std::string& relativePath) const; private: std::string root_; }; }
