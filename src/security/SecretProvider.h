#pragma once

#include "RuntimeSecret.h"

#include <string>

namespace dnxr {

class SecretProvider {
public:
    static RuntimeSecret fromEnvironment(const std::string& secretId, const std::string& environmentVariable);
    static RuntimeSecret simulationSessionSecret(const std::string& secretId, const std::string& sessionId = "default");
    static RuntimeSecret resolve(const std::string& secretId, const std::string& environmentVariable, bool allowSimulationFallback = true);
};

} // namespace dnxr
