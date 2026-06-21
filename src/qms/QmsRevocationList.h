#pragma once
#include <string>
namespace dnxr { class QmsRevocationList { public: explicit QmsRevocationList(std::string root = "."); bool isRevoked(const std::string& keyId) const; private: std::string root_; }; }
