#include "SecretProvider.h"

#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace dnxr {
namespace {

std::uint64_t fnv1a64(const std::string& material) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : material) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::string deterministicSimulationMaterial(const std::string& secretId, const std::string& sessionId) {
    const std::string seed = "dnxr-simulation-secret:" + secretId + ":" + sessionId;
    std::ostringstream oss;
    oss << "sim-" << std::hex << fnv1a64(seed) << fnv1a64(seed + ":neural-xr");
    return oss.str();
}

} // namespace

RuntimeSecret SecretProvider::fromEnvironment(
    const std::string& secretId,
    const std::string& environmentVariable) {

    const char* value = std::getenv(environmentVariable.c_str());
    if (value == nullptr || std::string(value).empty()) {
        return RuntimeSecret{secretId, "", SecretOrigin::Missing};
    }
    return RuntimeSecret{secretId, value, SecretOrigin::Environment};
}

RuntimeSecret SecretProvider::deterministicSimulationSecret(
    const std::string& secretId,
    const std::string& sessionId) {

    return RuntimeSecret{secretId, deterministicSimulationMaterial(secretId, sessionId), SecretOrigin::SimulationDeterministic};
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
        return deterministicSimulationSecret(secretId, environmentVariable);
    }
    throw std::runtime_error("missing required runtime secret: " + secretId);
}

RuntimeSecret SecretProvider::simulationSessionSecret(
    const std::string& secretId,
    const std::string& sessionId) {

    return deterministicSimulationSecret(secretId, sessionId);
}

} // namespace dnxr
