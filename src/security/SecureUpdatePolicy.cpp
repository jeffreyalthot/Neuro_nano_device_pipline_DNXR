#include "SecureUpdatePolicy.h"
namespace dnxr { bool SecureUpdatePolicy::acceptsUpdate(const std::string& signature, int monotonicVersion, int currentVersion) const { return signature.size() >= 16 && monotonicVersion > currentVersion; } }
