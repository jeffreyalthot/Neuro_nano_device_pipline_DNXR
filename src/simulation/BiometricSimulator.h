#pragma once

#include "NeuralSignalSimulator.h"

#include <cstdint>
#include <vector>

namespace dnxr {

struct BiometricSample {
    float heartRateBpm = 72.0f;
    float respiratoryRateRpm = 16.0f;
    float focusIndex = 0.85f;
    float neuralNoise = 0.02f;
    float oxygenLevel = 0.98f;
    float eyeStability = 0.90f;

    std::vector<float> toTelemetryVector() const;
};

class BiometricSimulator {
public:
    explicit BiometricSimulator(SimulationScenario scenario = SimulationScenario::NominalFocus);

    BiometricSample sample(std::uint64_t cycle) const;
    void setScenario(SimulationScenario scenario);
    SimulationScenario scenario() const;

private:
    SimulationScenario scenario_;
};

} // namespace dnxr
