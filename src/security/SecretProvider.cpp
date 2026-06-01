#include "SecretProvider.h"

#include <cstdlib>
#include <functional>
#include <sstream>
#include <stdexcept>

namespace dnxr {
namespace {

std::string stableSimulationMaterial(const std::string& secretId, const std::string& sessionId) {
    const std::string seed = "dnxr-simulation-secret:" + secretId + ":" + sessionId;
    const std::size_t h1 = std::hash<std::string>{}(seed);
    const std::size_t h2 = std::hash<std::string>{}(seed + ":neural-xr");
    std::ostringstream oss;
    oss << "sim-" << std::hex << h1 << h2;
    return oss.str();
}

} // namespace

RuntimeSecret SecretProvider::fromEnvironment(
    const std::string& secretId,
    const std::string& environmentVariable) {

    const char* value = std::getenv(environmentVariable.c_str());
    if (value == nullptr || std::string(value).empty()) {
        return RuntimeSecret{secretId, "", false};
    }
    return RuntimeSecret{secretId, value, false};
}

RuntimeSecret SecretProvider::simulationSessionSecret(
    const std::string& secretId,
    const std::string& sessionId) {

    return RuntimeSecret{secretId, stableSimulationMaterial(secretId, sessionId), true};
}

RuntimeSecret SecretProvider::resolve(
    const std::string& secretId,
    const std::string& environmentVariable,
    bool allowSimulationFallback) {

    RuntimeSecret secret = fromEnvironment(secretId, environmentVariable);
    if (!secret.empty()) {
        return secret;
    }
    if (allowSimulationFallback) {
        return simulationSessionSecret(secretId, environmentVariable);
    }
    throw std::runtime_error("missing required runtime secret: " + secretId);
}

} // namespace dnxr
