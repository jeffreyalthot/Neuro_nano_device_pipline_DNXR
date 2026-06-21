#pragma once
#include <string>
#include <vector>
namespace dnxr { class QmsTrustStore { public: explicit QmsTrustStore(std::string root = "."); bool isTrustedKey(const std::string& keyId) const; private: std::string root_; }; }
