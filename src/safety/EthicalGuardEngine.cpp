#include "EthicalGuardEngine.h"

#include <functional>
#include <numeric>

namespace dnxr {

bool EthicalGuardEngine::enforceNonBypassableLimits(const std::vector<float>& telemetry, const std::string& inferredState) const {
    const float avgStress = telemetry.empty() ? 0.0F : std::accumulate(telemetry.begin(), telemetry.end(), 0.0F) / static_cast<float>(telemetry.size());
    const bool dissociationRisk = inferredState.find("dissociation") != std::string::npos;
    return avgStress < 0.85F && !dissociationRisk;
}

std::string EthicalGuardEngine::signedAuditRecord(const std::string& action, const std::vector<float>& values) const {
    const float aggregate = std::accumulate(values.begin(), values.end(), 0.0F);
    const std::string payload = action + "|" + std::to_string(aggregate);
    const auto signature = std::hash<std::string>{}(payload);
    return payload + "|sig=" + std::to_string(signature);
}

} // namespace dnxr
