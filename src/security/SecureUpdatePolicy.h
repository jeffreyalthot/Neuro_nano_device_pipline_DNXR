#pragma once
#include <string>
namespace dnxr { class SecureUpdatePolicy { public: bool acceptsUpdate(const std::string& signature, int monotonicVersion, int currentVersion) const; }; }
