#pragma once

#include <string>
#include <vector>

namespace dnxr {

class EthicalGuardEngine {
public:
    bool enforceNonBypassableLimits(const std::vector<float>& telemetry, const std::string& inferredState) const;
    std::string signedAuditRecord(const std::string& action, const std::vector<float>& values) const;
};

} // namespace dnxr
