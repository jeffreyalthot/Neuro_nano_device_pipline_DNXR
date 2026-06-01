#pragma once

#include <algorithm>
#include <string>

namespace dnxr {

enum class SecretOrigin {
    Environment,
    SimulationDeterministic,
    Missing
};

struct RuntimeSecret {
    std::string id;
    std::string value;
    SecretOrigin origin = SecretOrigin::Missing;

    bool empty() const { return value.empty(); }
    bool simulationGenerated() const { return origin == SecretOrigin::SimulationDeterministic; }

    void clear() {
        std::fill(value.begin(), value.end(), '\0');
        value.clear();
        origin = SecretOrigin::Missing;
    }
};

} // namespace dnxr
